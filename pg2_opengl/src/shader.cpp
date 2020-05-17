#include "pch.h"
#include "shader.h"
#include "utils.h"

#include "log.h"

bool LoadAndCompileShader(const char* shaderPath, GLenum shaderType, GLuint& shaderHandle);
char* LoadShader(const char* file_name);
GLint CheckShader(const GLenum shader);

//================================= Shader =================================

ShaderProgram::ShaderProgram(const char* vShaderPath, const char* fShaderPath) {
	GLuint vShader, fShader;

	if (!LoadAndCompileShader(vShaderPath, GL_VERTEX_SHADER, vShader)) {
		errlog("Shader failed to compile ('%s' - vertex)\n", vShaderPath);
		glDeleteShader(vShader);
		throw std::exception("Shader program failed to compile.");
	}

	if (!LoadAndCompileShader(fShaderPath, GL_FRAGMENT_SHADER, fShader)) {
		errlog("Shader failed to compile ('%s' - fragment)\n", fShaderPath);
		glDeleteShader(vShader);
		glDeleteShader(fShader);
		throw std::exception("Shader program failed to compile.");
	}

	programID = glCreateProgram();
	glAttachShader(programID, vShader);
	glAttachShader(programID, fShader);
	glLinkProgram(programID);
	// TODO check linking
	glUseProgram(programID);

	glDeleteShader(vShader);
	glDeleteShader(fShader);

	errlog("Shader compilation successful ('%s').\n", vShaderPath);
}

ShaderProgram::~ShaderProgram() {
	glDeleteProgram(programID);
	programID = 0;
}

ShaderProgram::ShaderProgram(ShaderProgram&& s) noexcept : programID(s.programID) {
	s.programID = 0;
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& s) noexcept {
	programID = s.programID;
	s.programID = 0;
	return *this;
}

void ShaderProgram::Bind() const {
	glUseProgram(programID);
}

#define shaderLog(logging, ...) if((logging)) { warnlog(__VA_ARGS__); }

void ShaderProgram::UploadMat3(const char* name, float* data, bool log) {
	const GLint location = glGetUniformLocation(programID, name);
	if (location == -1) { shaderLog(log, "Matrix '%s' not found in active shader.\n", name); }
	else glUniformMatrix3fv(location, 1, GL_TRUE, data);
}

void ShaderProgram::UploadMat4(const char* name, float* data, bool log) {
	const GLint location = glGetUniformLocation(programID, name);
	if (location == -1) { shaderLog(log, "Matrix '%s' not found in active shader.\n", name); }
	else glUniformMatrix4fv(location, 1, GL_TRUE, data);
}

void ShaderProgram::UploadFloat3(const char* name, float* data, bool log) {
	const GLint location = glGetUniformLocation(programID, name);
	if (location == -1) { shaderLog(log, "Variable '%s' not found in active shader.\n", name); }
	else glUniform3fv(location, 1, data);
}

void ShaderProgram::UploadFloat4(const char* name, float* data, bool log) {
	const GLint location = glGetUniformLocation(programID, name);
	if (location == -1) { shaderLog(log, "Variable '%s' not found in active shader.\n", name); }
	else glUniform4fv(location, 1, data);
}

void ShaderProgram::UploadInt(const char* name, int data, bool log) {
	const GLint location = glGetUniformLocation(programID, name);
	if (location == -1) { shaderLog(log, "Variable '%s' not found in active shader.\n", name); }
	else glUniform1i(location, data);
}

void ShaderProgram::UploadFloat(const char* name, float data, bool log) {
	const GLint location = glGetUniformLocation(programID, name);
	if (location == -1) { shaderLog(log, "Variable '%s' not found in active shader.\n", name); }
	else glUniform1f(location, data);
}

void ShaderProgram::UploadARBHandle(const char* name, GLuint64 data, bool log) {
	const GLint location = glGetUniformLocation(programID, name);
	if (location == -1) { shaderLog(log, "Variable '%s' not found in active shader.\n", name); }
	else glUniformHandleui64ARB(location, data);
}

//================================= Loading functions =================================

bool LoadAndCompileShader(const char* shaderPath, GLenum shaderType, GLuint& shaderHandle) {
	shaderHandle = glCreateShader(shaderType);
	const char* shaderSource = LoadShader(shaderPath);
	glShaderSource(shaderHandle, 1, &shaderSource, nullptr);
	glCompileShader(shaderHandle);
	SAFE_DELETE_ARRAY(shaderSource);
	return CheckShader(shaderHandle) == GL_TRUE;
}

/* load shader code from text file */
char* LoadShader(const char* file_name) {
	FILE* file = fopen(file_name, "rt");

	if (file == NULL) {
		errlog("IO error: File '%s' not found.\n", file_name);

		return NULL;
	}

	size_t file_size = static_cast<size_t>(GetFileSize64(file_name));
	char* shader = NULL;

	if (file_size < 1) {
		errlog("Shader error: File '%s' is empty.\n", file_name);
	}
	else {
		/* v glShaderSource nezadáváme v posledním parametru délku,
		takže øetìzec musí být null terminated, proto +1 a reset na 0*/
		shader = new char[file_size + 1];
		memset(shader, 0, sizeof(*shader) * (file_size + 1));

		size_t bytes = 0; // poèet již naètených bytù

		do {
			bytes += fread(shader, sizeof(char), file_size, file);
		} while (!feof(file) && (bytes < file_size));

		if (!feof(file) && (bytes != file_size)) {
			errlog("IO error: Unexpected end of file '%s' encountered.\n", file_name);
		}
	}

	fclose(file);
	file = NULL;

	return shader;
}

/* check shader for completeness */
GLint CheckShader(const GLenum shader) {
	GLint status = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE) {
		int info_length = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_length);
		char* info_log = new char[info_length];
		memset(info_log, 0, sizeof(*info_log) * info_length);
		glGetShaderInfoLog(shader, info_length, &info_length, info_log);

		errlog("Error log: %s\n", info_log);

		SAFE_DELETE_ARRAY(info_log);
	}

	return status;
}
