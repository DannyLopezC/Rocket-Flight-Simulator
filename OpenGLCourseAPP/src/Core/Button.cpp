#include "Button.h"

Button::Button(float width, float height, Material& mat, Texture& tex)
{
	pos = glm::vec3(0.0f, 0.0f, 0.0f);
	scale = glm::vec3(1.0f, 1.0f, 1.0f);
	normalScale = scale;
	rotation = glm::vec3(0.0f, 0.0f, 1.0f);
	rotationAngle = 0;

	this->width = width;
	this->height = height;

	buttonMesh = PrimitiveFactory::createUIQuad(width, height);

	this->mat = mat;
	this->tex = tex;

	hoverColorMultiplier = 0.7f;
	pressedColorMultiplier = 0.5f;

	color = glm::vec4(1, 1, 1, 1);
	
	normalColor = color;
	hoverColor = glm::vec4(color.r * hoverColorMultiplier, color.g * hoverColorMultiplier, color.b * hoverColorMultiplier, color.a);
	pressedColor = glm::vec4(color.r * pressedColorMultiplier, color.g * pressedColorMultiplier, color.b * pressedColorMultiplier, color.a);

	hovered = false;
	pressed = false;
}

void Button::setPos(glm::vec3 pos)
{
	this->pos = pos;
}

void Button::setScale(glm::vec3 scale)
{
	this->scale = scale;
	normalScale = scale;
}

void Button::setRotation(glm::vec3 rotation, float rotationAngle)
{
	this->rotation = rotation;
	this->rotationAngle = rotationAngle;
}

void Button::setColor(glm::vec4 color)
{
	this->color = color;
	normalColor = color;
	hoverColor = glm::vec4(color.r * hoverColorMultiplier, color.g * hoverColorMultiplier, color.b * hoverColorMultiplier, color.a);
	pressedColor = glm::vec4(color.r * pressedColorMultiplier, color.g * pressedColorMultiplier, color.b * pressedColorMultiplier, color.a);
}

void Button::updateButtonState(float mx, float my, bool mouseDown)
{
	float left = pos.x - (width * scale.x) / 2;
	float right = pos.x + (width * scale.x ) / 2;
	float bottom = pos.y - (height * scale.y) / 2;
	float up = pos.y + (height * scale.y) / 2;

	hovered = mx >= left && mx <= right && my >= bottom && my <= up;
	pressed = hovered && mouseDown;

	if (pressed) pressedAnimation();
	else if (hovered) { color = hoverColor; scale = normalScale; }
	else idleAnimation();
}

void Button::render(GLuint uniformModel, GLuint colorLoc)
{
	glm::mat4 model(1.0f);

	model = glm::mat4(1.0f);
	model = glm::translate(model, pos);
	
	if (fabs(rotationAngle) > 0.0001f) {
		model = glm::rotate(model, rotationAngle, rotation);
	}

	model = glm::scale(model, scale);

	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	glUniform4fv(colorLoc, 1, glm::value_ptr(color));

	mat.useMaterial(0, 0);
	tex.useTexture();

	buttonMesh->renderMesh();
}

void Button::pressedAnimation()
{
	this->color = pressedColor;
	scale = normalScale * 0.95f;
}

void Button::idleAnimation()
{
	this->color = normalColor;
	scale = normalScale;
}

Button::~Button()
{
	delete buttonMesh;
	buttonMesh = nullptr;
}
