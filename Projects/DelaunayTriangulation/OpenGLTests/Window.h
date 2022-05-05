#pragma once

#include <stdio.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>


class Window
{
public:
	Window();
	Window(GLint windowWidth, GLint windowHeight);

	int Initialise();

	GLint GetBufferWidth() { return bufferWidth; }
	GLint GetBufferHeight() { return bufferHeight; }

	bool GetShouldClose() { return glfwWindowShouldClose(mainWindow); }

	void SwapBuffers() { glfwSwapBuffers(mainWindow); }

	GLFWwindow* GetGLFWwindow() { return mainWindow; }

	~Window();

private:
	GLFWwindow* mainWindow;

	GLint width, height, bufferWidth, bufferHeight;


};

