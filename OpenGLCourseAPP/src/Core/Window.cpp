#include "Window.h"

Window::Window()
{
    width = 800;
    height = 600;
	bufferWidth = 0;
	bufferHeight = 0;
	mainWindow = nullptr;
}

Window::Window(GLint windowWidth, GLint windowHeight)
{
    width = windowWidth;
    height = windowHeight;
	bufferWidth = 0;
	bufferHeight = 0;
	mainWindow = nullptr;
}

int Window::initialise()
{
	// Init GLFW
	if (!glfwInit())
	{
		printf("GLFW Init failed!");
		glfwTerminate();
		return 1;
	}

	// Setup GLFW window properties
	// OpenGL version

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	// Core profile = No Backwords compatibility
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Allow forward compatibility
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	mainWindow = glfwCreateWindow(width, height, "Test window", NULL, NULL);
	if (!mainWindow)
	{
		printf("Window creating failed!");
		glfwTerminate();
		return 1;
	}

	// Get buffer size Information
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	// set context for GLEW
	glfwMakeContextCurrent(mainWindow);

	//glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// allow mordern extension features
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("Glew init failed!");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEPTH_TEST);

	// setup viewport size
	glViewport(0, 0, bufferWidth, bufferHeight);
}

Window::~Window()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
}