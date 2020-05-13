#include "pch.h"
#include "scene.h"

#include "log.h"
#include "objloader.h"

//Vytvori a naplni buffer s daty pro VBO.
std::pair<int, float*> MergeSurfaces(std::vector<Surface*>& surfaces, std::vector<Mesh>& meshes);
//Vytvori a naplni buffer obsahujici materialy.
GLMaterial* ParseMaterials(std::vector<Material*>& materials);

//================================= Scene =================================

Scene::Scene(const char* filepath) {
	if (strcmp(filepath, "default") == 0) {
		LoadDefault();
	}
	else if (!Load(filepath)) {
		throw std::exception("Scene failed to load.");
	}
}

Scene::~Scene() {
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ssbo);
	glDeleteVertexArrays(1, &vao);
	vao = vbo = ssbo = 0;
}

Scene::Scene(Scene&& s) noexcept 
	: vao(s.vao), vbo(s.vbo), ssbo(s.ssbo), meshes(s.meshes), materials(s.materials), vertexCount(s.vertexCount) {
	s.vao = s.vbo = s.ssbo = 0;
	s.meshes.clear();
	s.materials.clear();
}

Scene& Scene::operator=(Scene&& s) noexcept {
	vao = s.vao;
	vbo = s.vbo;
	ssbo = s.ssbo;
	meshes = s.meshes;
	materials = s.materials;
	vertexCount = s.vertexCount;

	s.vao = s.vbo = s.ssbo = 0;
	s.meshes.clear();
	s.materials.clear();

	return *this;
}

void Scene::Draw() const {
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, vertexCount);
	/*for (auto& m : meshes)
		m.Draw();*/
}

bool Scene::Load(const char* filepath) {
	//load scene data from file
	std::vector<Surface*> surfaces;
	if (LoadOBJ(filepath, surfaces, materials, false) < 0) {
		errlog("Failed to load scene '%s'.\n", filepath);
		return false;
	}

	//merge surfaces into a single array of vertices
	auto [vc, vertexData] = MergeSurfaces(surfaces, meshes);
	vertexCount = vc;

	//convert materials
	glMaterials = ParseMaterials(materials);

	//generate VAO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//generate & fill VBO
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(Vertex), vertexData, GL_STATIC_DRAW);

	//Setup vertex attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, position)));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, normal)));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, color)));
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, texture_coords)));
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, tangent)));
	glVertexAttribIPointer(5, 1, GL_INT,			sizeof(Vertex), (void*)(offsetof(Vertex, matIdx)));
	for (int i = 0; i < 6; i++)
		glEnableVertexAttribArray(i);

	//SSBO
	glGenBuffers(1, &ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLMaterial) * materials.size(), (float*)glMaterials, GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

	//cleanup
	for (Surface* s : surfaces)
		delete s;
	delete[] vertexData;

	errlog("Scene '%s' loaded.\n", filepath);
	return true;
}

void Scene::LoadDefault() {
	errlog("Loading default scene.\n");

	// setup vertex buffer as AoS (array of structures)
	GLfloat vertices[] = {
		-0.9f, 0.9f, 0.0f,  0.0f, 1.0f, // vertex 0 : p0.x, p0.y, p0.z, t0.u, t0.v
		0.9f, 0.9f, 0.0f,   1.0f, 1.0f, // vertex 1 : p1.x, p1.y, p1.z, t1.u, t1.v
		0.0f, -0.9f, 0.0f,  0.5f, 0.0f  // vertex 2 : p2.x, p2.y, p2.z, t2.u, t2.v
	};
	const int no_vertices = 3;
	const int vertex_stride = sizeof(vertices) / no_vertices;

	//VAO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//VBO
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Vertex attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertex_stride, 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, vertex_stride, (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);

	vertexCount = no_vertices;

	errlog("Default scene loaded.\n");
}

//================================= Mesh =================================

Mesh::Mesh(int o, int c, Material* m) : offset(o), count(c), material(m) {}

void Mesh::Draw() const {
	glDrawArrays(GL_TRIANGLES, offset, count);
}

//================================= Parse methdos =================================

GLMaterial* ParseMaterials(std::vector<Material*>& materials) {
	GLMaterial* data = new GLMaterial[materials.size()];

	int i = 0;
	for (Material* m : materials) {
		data[i++] = m->GenerateGLMaterial();
	}

	return data;
}

std::pair<int, float*> MergeSurfaces(std::vector<Surface*>& surfaces, std::vector<Mesh>& meshes) {
	//secte vsechny trojuhelniky ve scene
	int totalTriangles = 0;
	for (Surface* s : surfaces) {
		s->setMaterialIdx();						//kazdemu vrcholu nastavi idx materialu
		totalTriangles += s->no_triangles();
	}

	//alokace bufferu pro data sceny
	Triangle* data = new Triangle[totalTriangles];

	//kopirovani jednotlivych povrchu do bufferu
	int offset = 0;
	for (Surface* s : surfaces) {
		int no_triangles = s->copyTriangles(data, offset);

		//mesh reprezentuje 1 povrch v ramci bufferu (offset a velikost, kterou v nem zabira + material)
		meshes.push_back(Mesh(offset * 3, no_triangles * 3, s->get_material()));

		offset += no_triangles;
	}

	return { totalTriangles * 3, (float*)data };
}
