#pragma once
#include <GLFW/glfw3.h>
#include <algorithm>
#include <vector>

class Input
{
public:
	Input() = default;

	void init(GLFWwindow* window);

	void beginFrame();
	void update();

	float getMouseX() const { return mouseX; }
	float getMouseY() const { return mouseY; }

	float getMouseXUI() const { return mouseX; }
	float getMouseYUI() const { return windowH - mouseY; }

	bool mouseDown(int button = GLFW_MOUSE_BUTTON_LEFT) const;
	bool mousePressed(int button = GLFW_MOUSE_BUTTON_LEFT) const;
	bool mouseReleased(int button = GLFW_MOUSE_BUTTON_LEFT) const;

	bool keyDown(int key) const;
	bool keyPressed(int key) const;
	bool keyReleased(int key) const;

	void setWindowSize(int w, int h);
	const std::vector<unsigned int>& getCharBuffer() const { return charBuffer; }

private:
	GLFWwindow* window = nullptr;

	float mouseX = 0.0f;
	float mouseY = 0.0f;
	float windowW = 1.0f;
	float windowH = 1.0f;

	bool mouseDownPrev[GLFW_MOUSE_BUTTON_LAST + 1]{};
	bool mouseDownNow[GLFW_MOUSE_BUTTON_LAST + 1]{};

	bool keyDownPrev[GLFW_KEY_LAST + 1]{};
	bool keyDownNow[GLFW_KEY_LAST + 1]{};

	std::vector<unsigned int> charBuffer;

	static void charCallback(GLFWwindow* window, unsigned int codepoint);
};