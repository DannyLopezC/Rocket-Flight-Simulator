#include "Arrow.h"

Arrow::Arrow(float length, glm::vec3 pos, glm::vec3 rotV, float angle, glm::vec3 scale)
{
	this->pos = pos;
	this->rotV = rotV;
	this->angle = angle;
	this->length = length;
	this->scale = { length, scale.y, scale.z };

	this->angleRad = glm::radians(angle);
	this->dir = glm::vec3(cos(angleRad), sin(angleRad), 0.0f);

	line = PrimitiveFactory::createLine();
	head = PrimitiveFactory::createArrowHead();
}

void Arrow::render(GLuint uniformModel)
{
	glm::mat4 model(1.0f);

	model = glm::mat4(1.0f);
	model = glm::translate(model, pos);
	model = glm::rotate(model, angleRad, rotV);
	model = glm::scale(model, scale);
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	line->renderLines();

	model = glm::mat4(1.0f);
	model = glm::translate(model, pos + dir * length);
	model = glm::rotate(model, angleRad, rotV);
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	head->renderLines();
}

void Arrow::setArrowLength(float length)
{
	this->length = length;
	scale.x = length;
}

void Arrow::setArrowPos(glm::vec3 pos)
{
	this->pos = pos;
}

void Arrow::setArrowAngle(float angle)
{
	this->angle = angle;
	angleRad = glm::radians(angle);
	dir = glm::vec3(cos(angleRad), sin(angleRad), 0.0f);
}

Arrow::~Arrow()
{
}
