#pragma once
#include "glm/glm.hpp"
class Ball
{
public:
	Ball(glm::vec2 startPos = { 0.0f, 5.0f },
		glm::vec2 startVel = { 0.0f, 0.0f },
		float r = 1.0f);

	void update(float dt, float gravity);
	void bounceOnFloor(float floorY, float restitution);

	glm::vec2 GetPos() const { return pos; }
	glm::vec2 GetVel() const { return vel; }
	float GetRadius() const { return radius; }

	void SetPos(glm::vec2 p) { pos = p; }
	void SetVel(glm::vec2 v) { vel = v; }

	~Ball();
private:
	glm::vec2 pos;
	glm::vec2 vel;
	float radius;

};

