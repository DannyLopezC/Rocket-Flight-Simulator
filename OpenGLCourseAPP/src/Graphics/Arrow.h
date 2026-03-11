#pragma once
#include "Graphics/Mesh.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Graphics/PrimitiveFactory.h"

class Arrow
{
public:
	Arrow(float length, glm::vec3 pos, glm::vec3 rotV, float angle, glm::vec3 scale = { 1.0f, 1.0f, 1.0f });

	void render(GLuint uniformModel);
	
	void setArrowLength(float length);
	void setArrowPos(glm::vec3 pos);
	void setArrowAngle(float angle);

	~Arrow();

private:
	Mesh* line;
	Mesh* head;

	float length;
	float angle;
	float angleRad;
	glm::vec3 pos;
	glm::vec3 rotV;
	glm::vec3 scale;
	glm::vec3 dir;

};

