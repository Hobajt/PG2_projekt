#pragma once

class ShaderProgram {
public:
	//invalid ctor
	ShaderProgram() {}
	ShaderProgram(const char* vShaderPath, const char* fShaderPath);
	~ShaderProgram();

	//copy deleted
	ShaderProgram(const ShaderProgram&) = delete;
	ShaderProgram& operator=(const ShaderProgram&) = delete;

	//move enabled
	ShaderProgram(ShaderProgram&&) noexcept;
	ShaderProgram& operator=(ShaderProgram&&) noexcept;

	void Bind() const;

	void UploadMat3(const char* name, float* data, bool log = true);
	void UploadMat4(const char* name, float* data, bool log = true);

	void UploadFloat3(const char* name, float* data, bool log = true);
	void UploadFloat4(const char* name, float* data, bool log = true);

	void UploadInt(const char* name, int data, bool log = true);
	void UploadFloat(const char* name, float data, bool log = true);

	void UploadARBHandle(const char* name, GLuint64 data, bool log = true);
private:
	unsigned int programID = 0;
};
