#include "Input.h"

void Input::init(GLFWwindow* window)
{
	this->window = window;

	std::fill(std::begin(mouseDownPrev), std::end(mouseDownPrev), false);
	std::fill(std::begin(mouseDownNow), std::end(mouseDownNow), false);

	std::fill(std::begin(keyDownPrev), std::end(keyDownPrev), false);
	std::fill(std::begin(keyDownNow), std::end(keyDownNow), false);

	double x, y;
	glfwGetCursorPos(window, &x, &y);
	mouseX = (float)x;
	mouseY = (float)y;
}

void Input::update()
{
	std::copy(std::begin(mouseDownNow), std::end(mouseDownNow), std::begin(mouseDownPrev));
	std::copy(std::begin(keyDownNow), std::end(keyDownNow), std::begin(keyDownPrev));

	double x, y;
	glfwGetCursorPos(window, &x, &y);
	mouseX = (float)x;
	mouseY = (float)y;

	for (int b = 0; b <= GLFW_MOUSE_BUTTON_LAST; ++b)
	{
		int state = glfwGetMouseButton(window, b);
		mouseDownNow[b] = (state == GLFW_PRESS);
	}

	for (int k = 0; k <= GLFW_KEY_LAST; ++k)
	{
		int state = glfwGetKey(window, k);
		keyDownNow[k] = (state == GLFW_PRESS || state == GLFW_REPEAT);
	}
}

bool Input::mouseDown(int button) const
{
	if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST) return false;
	return mouseDownNow[button];
}

bool Input::mousePressed(int button) const
{
	if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST) return false;
	return mouseDownNow[button] && !mouseDownPrev[button];
}

bool Input::mouseReleased(int button) const
{
	if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST) return false;
	return !mouseDownNow[button] && mouseDownPrev[button];
}

bool Input::keyDown(int key) const
{
	if (key < 0 || key > GLFW_KEY_LAST) return false;
	return keyDownNow[key];
}

bool Input::keyPressed(int key) const
{
	if (key < 0 || key > GLFW_KEY_LAST) return false;
	return keyDownNow[key] && !keyDownPrev[key];
}

bool Input::keyReleased(int key) const
{
	if (key < 0 || key > GLFW_KEY_LAST) return false;
	return !keyDownNow[key] && keyDownPrev[key];
}

void Input::setWindowSize(int w, int h)
{
	windowW = (float)w;
	windowH = (float)h;
}
