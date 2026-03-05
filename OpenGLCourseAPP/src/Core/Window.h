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

	// CHANGE THIS FOR BETTER PATTERNS
	bool* getKeys() { return keys; }
	GLfloat getXChange();
	GLfloat getYChange();
	//

	void swapBuffers() { glfwSwapBuffers(mainWindow); }
	
	~Window();

private:
	GLFWwindow* mainWindow;

	GLint width, height;
	GLint bufferWidth, bufferHeight;

	bool keys[1024];

	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	bool mousedFirstMoved;

	void createCallbacks();
	static void handleKeys(GLFWwindow* window, int key, int code, int action, int mode);
	static void handleMouse(GLFWwindow* window, double xPos, double yPos);
};

