#pragma once

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

#include <GL/glew.h>

class Shader
{
public:
	Shader();
	~Shader();

	void CreateFromString(const char* vertexCode, const char* fragmentCode);
	void CreateFromFiles(const char* vertexLocation, const char* fragmentLocation);



	GLuint GetProjectionLocation();
	GLuint GetModelLocation();
	GLuint GetMyColorLocation();

	void UseShader();
	void ClearShader();


private:
	GLuint shaderID, uniformProjection, uniformModel, uniformMyColor;

	void CompileShader(const char* vertexCode, const char* fragmentCode);
	void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType);

	std::string ReadFile(const char* fileLocation);

};

