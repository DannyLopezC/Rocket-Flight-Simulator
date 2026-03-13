#include "InputField.h"

InputField::InputField(float width, float height, Texture& outTex)
{
	pos = glm::vec3(0.0f, 0.0f, 0.0f);
	scale = glm::vec3(1.0f, 1.0f, 1.0f);
	rotation = glm::vec3(0.0f, 0.0f, 1.0f);
	rotationAngle = 0;

	this->width = width;
	this->height = height;

	outInputFieldMesh = PrimitiveFactory::createUIQuad(width, height);

	this->outTex = outTex;

	hoverColorMultiplier = 0.8f;
	focusedColorMultiplier = 0.7f;

	color = glm::vec4(1, 1, 1, 1);
	textColor = glm::vec4(0, 0, 0, 1);

	normalColor = color;
	hoverColor = glm::vec4(color.r * hoverColorMultiplier, color.g * hoverColorMultiplier, color.b * hoverColorMultiplier, color.a);
	focusedColor = glm::vec4(color.r * focusedColorMultiplier, color.g * focusedColorMultiplier, color.b * focusedColorMultiplier, color.a);

	state = IDLE;

	currentText = "";
	maxCharacters = 0;
	textRenderer = new TextRenderer();
}

void InputField::initTextRenderer(unsigned int screenWidth, unsigned int screenHeight, Shader* textShader,
	const char* fontPath, glm::vec4 textColor, int maxCharacters)
{
	textRenderer->init(screenWidth, screenHeight, textShader);
	textRenderer->loadFont(fontPath, 24);
	this->textColor = textColor;
	this->maxCharacters = maxCharacters;
}

void InputField::setPos(glm::vec3 pos)
{
	this->pos = pos;
}

void InputField::setScale(glm::vec3 scale)
{
	this->scale = scale;
}

void InputField::setRotation(glm::vec3 rotation, float rotationAngle)
{
	this->rotation = rotation;
	this->rotationAngle = rotationAngle;
}

void InputField::setColor(glm::vec4 color)
{
	this->color = color;
	this->normalColor = color;
	hoverColor = glm::vec4(color.r * hoverColorMultiplier, color.g * hoverColorMultiplier, color.b * hoverColorMultiplier, color.a);
	focusedColor = glm::vec4(color.r * focusedColorMultiplier, color.g *  focusedColorMultiplier, color.b *  focusedColorMultiplier, color.a);
}

void InputField::setText(std::string txt)
{
	currentText = txt;
}

void InputField::updateInputFieldState(float mx, float my, bool clicked)
{
	float left = pos.x - (width * scale.x) / 2;
	float right = pos.x + (width * scale.x) / 2;
	float bottom = pos.y - (height * scale.y) / 2;
	float up = pos.y + (height * scale.y) / 2;

	bool inside = (mx >= left && mx <= right && my >= bottom && my <= up);

	if (clicked)
	{
		isFocused = inside;
	}

	if (isFocused)
	{
		state = FOCUSED;
	}
	else if (inside)
	{
		state = HOVER;
	}
	else
	{
		state = IDLE;
	}

	switch (state)
	{
	case IDLE:
		color = normalColor;
		break;
	case HOVER:
		color = hoverColor;
		break;
	case FOCUSED:
		writingAnimation();
		break;
	default:
		break;
	}
}

void InputField::render(GLuint uniformModel, GLuint colorLoc)
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

	outTex.useTexture();
	outInputFieldMesh->renderMesh();

	textRenderer->renderText(currentText, pos.x - (width/2) + 5.0f, pos.y - 5.0f, 0.8f, textColor);
}

void InputField::writeChar(char c)
{
	if (state != FOCUSED || currentText.length() >= maxCharacters) return;

	if (c >= 32 && c <= 126)
	{
		currentText.push_back(c);
	}
}

void InputField::writeKey(int c)
{
	if (state != FOCUSED) return;
	switch (c)
	{
	case GLFW_KEY_BACKSPACE:
		if (!currentText.empty()) currentText.pop_back();
		break;
	case GLFW_KEY_ENTER:
		isFocused = false;
		state = IDLE;
		break;
	case GLFW_KEY_ESCAPE:
		isFocused = false;
		state = IDLE;
		break;
	default:
		break;
	}
}

void InputField::writingAnimation()
{
	this->color = focusedColor;
}

InputField::~InputField()
{
	delete outInputFieldMesh;
	outInputFieldMesh = nullptr;

	delete textRenderer;
	textRenderer = nullptr;
}
