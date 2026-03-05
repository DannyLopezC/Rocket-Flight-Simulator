#pragma once
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Graphics/Texture.h"
#include "Graphics/PrimitiveFactory.h"
#include "Graphics/Material.h"
#include <iostream>

class Button
{
public:
	Button(float width, float height, Material& mat, Texture& tex);

	void setPos(glm::vec3 pos);
	void setScale(glm::vec3 scale);
	void setRotation(glm::vec3 rotation, float rotationAngle);

	void render(GLuint uniformModel, GLuint colorLoc);
	void setColor(glm::vec4 color);

	bool getButtonHovered() const { return hovered; }

	void updateButtonState(float mx, float my, bool clicked);

	glm::vec3 getPos() const { return pos; }
	glm::vec3 getScale() const { return scale; }
	glm::vec3 getRotation() const { return rotation; }

	~Button();
private:
	glm::vec3 pos;
	glm::vec3 scale;
	glm::vec3 normalScale;
	glm::vec3 rotation;
	float rotationAngle;

	float width;
	float height;

	Texture tex;
	Material mat;

	glm::vec4 color;

	glm::vec4 normalColor;
	glm::vec4 hoverColor;
	float hoverColorMultiplier;
	glm::vec4 pressedColor;
	float pressedColorMultiplier;

	Mesh* buttonMesh;

	bool pressed, hovered;

	void pressedAnimation();
	void idleAnimation();
};

