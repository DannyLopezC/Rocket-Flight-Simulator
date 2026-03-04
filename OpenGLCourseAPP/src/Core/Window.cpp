#include "Window.h"

Window::Window()
{
    width = 800;
    height = 600;
	bufferWidth = 0;
	bufferHeight = 0;
	mainWindow = nullptr;

	// input variables
	lastX = 0.0f;
	lastY = 0.0f;
	xChange = 0.0f;
	yChange = 0.0f;
	mousedFirstMoved = false;

	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}

Window::Window(GLint windowWidth, GLint windowHeight)
{
    width = windowWidth;
    height = windowHeight;
	bufferWidth = 0;
	bufferHeight = 0;
	mainWindow = nullptr;

	// input variables
	lastX = 0.0f;
	lastY = 0.0f;
	xChange = 0.0f;
	yChange = 0.0f;
	mousedFirstMoved = false;
	
	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
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

	// handle callbacks (like keys)
	createCallbacks();
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

	glfwSetWindowUserPointer(mainWindow, this);
}

void Window::createCallbacks()
{
	glfwSetKeyCallback(mainWindow, handleKeys);
	glfwSetCursorPosCallback(mainWindow, handleMouse);
}

void Window::handleKeys(GLFWwindow* window, int key, int code, int action, int mode)
{
	Window* localWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS) {
			localWindow->keys[key] = true;
		}else if(action == GLFW_RELEASE) {
			localWindow->keys[key] = false;
		}
	}
}

void Window::handleMouse(GLFWwindow* window, double xPos, double yPos)
{
	Window* localWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (localWindow->mousedFirstMoved) {
		localWindow->lastX = xPos;
		localWindow->lastY = yPos;
		localWindow->mousedFirstMoved = false;
	}

	localWindow->xChange = xPos - localWindow->lastX;
	localWindow->yChange = localWindow->lastY - yPos;

	localWindow->lastX = xPos;
	localWindow->lastY = yPos;
}

GLfloat Window::getXChange()
{
	GLfloat localChange = xChange;
	xChange = 0.0f;
	return localChange;
}

GLfloat Window::getYChange()
{
	GLfloat localChange = yChange;
	yChange = 0.0f;
	return localChange;
}

Window::~Window()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
}