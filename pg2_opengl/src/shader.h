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

	void UploadMat3(const char* name, float* data);
	void UploadMat4(const char* name, float* data);

	void UploadFloat3(const char* name, float* data);
	void UploadFloat4(const char* name, float* data);

	void UploadInt(const char* name, int data);
	void UploadFloat(const char* name, float data);
private:
	unsigned int programID = 0;
};
