#pragma once

#include <vector>

class Material;
struct GLMaterial;

class Mesh {
public:
	Mesh(int offset, int count, Material* material);

	void Draw() const;
public:
	int offset;
	int count;
	Material* material;
};

class Scene {
public:
	Scene() {}			//invalid constructor
	Scene(const char* filepath);
	~Scene();

	//copy deleted
	Scene(const Scene&) = delete;
	Scene& operator=(const Scene&) const = delete;

	//move enabled
	Scene(Scene&&) noexcept;
	Scene& operator=(Scene&&) noexcept;

	void Draw() const;
private:
	bool Load(const char* filepath);
	void LoadDefault();
private:
	std::vector<Material*> materials;
	std::vector<Mesh> meshes;

	int vertexCount = 0;

	GLuint vao  = 0;
	GLuint vbo  = 0;
	GLuint ssbo = 0;

	GLMaterial* glMaterials = nullptr;
};
