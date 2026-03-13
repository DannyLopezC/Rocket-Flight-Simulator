#pragma once
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "gl/glew.h"
#include "GLFW/glfw3.h"

#include "Graphics/PrimitiveFactory.h"
#include "Graphics/Material.h"
#include "Graphics/Texture.h"
#include "Graphics/TextRenderer.h"

enum INPUT_FIELD_STATE {
	IDLE = 0,
	HOVER = 1,
	FOCUSED = 2
};

class InputField
{
public:
	InputField(float width, float height, Texture& outTex);
	void initTextRenderer(unsigned int screenWidth, unsigned int screenHeight, Shader* textShader,
		const char* fontPath, glm::vec4 textColor, int maxCharacters);

	void setPos(glm::vec3 pos);
	void setScale(glm::vec3 scale);
	void setRotation(glm::vec3 rotation, float rotationAngle);
	void setColor(glm::vec4 color);
	void setText(std::string txt);

	bool getInputFieldHovered() const { return state == HOVER; }
	bool getInputFieldFocused() const { return isFocused; }
	std::string getCurrentText() const { return currentText; } 

	glm::vec3 getPos() const { return pos; }
	glm::vec3 getScale() const { return scale; }
	glm::vec3 getRotation() const { return rotation; }
	float getWidth() const { return width; }
	float getHeight() const { return height; }

	void render(GLuint uniformModel, GLuint colorLoc);

	void updateInputFieldState(float mx, float my, bool clicked);
	void writeChar(char c);
	void writeKey(int c);

	~InputField();

private:
	glm::vec3 pos;
	glm::vec3 scale;
	glm::vec3 rotation;
	float rotationAngle;

	float width;
	float height;

	Texture outTex;

	glm::vec4 color;
	glm::vec4 textColor;

	glm::vec4 normalColor;
	glm::vec4 hoverColor;
	float hoverColorMultiplier;
	glm::vec4 focusedColor;
	float focusedColorMultiplier;

	Mesh* outInputFieldMesh;

	INPUT_FIELD_STATE state;
	bool isFocused = false;

	void writingAnimation();

	std::string currentText;
	int maxCharacters;
	TextRenderer* textRenderer;
};

