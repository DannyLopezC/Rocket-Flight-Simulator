#pragma once

#include <stdio.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>


class Window
{
public:
	Window();
	Window(GLint windowWidth, GLint windowHeight);

	int initialise();

	GLint getBufferWidth() const { return bufferWidth; }
	GLint getBufferHeight() const { return bufferHeight; }
	GLFWwindow* getWindow() const { return mainWindow; }

	bool getShouldClose() { return glfwWindowShouldClose(mainWindow); }

	void swapBuffers() { glfwSwapBuffers(mainWindow); }
	
	~Window();

private:
	GLFWwindow* mainWindow;

	GLint width, height;
	GLint bufferWidth, bufferHeight;
};

