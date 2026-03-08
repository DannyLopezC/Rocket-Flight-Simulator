#pragma once
#include "glm/glm.hpp"
#include <iostream>
class Ball
{
public:
	Ball(glm::vec2 startPos = { 0.0f, 5.0f },
		glm::vec2 startVel = { 0.0f, 0.0f },
		float r = 1.0f);
	
	void restart();

	glm::vec2 getPos() const { return pos; }
	glm::vec2 getVel() const { return vel; }
	float getRadius() const { return radius; }

	void setXPos(float xPos) { pos.x = xPos; }
	void setYPos(float yPos) { pos.y = yPos; }
	void setPos(glm::vec2 p) { pos = p; }

	void setXVel(float xVel) { vel.x = xVel; }
	void setYVel(float yVel) { vel.y = yVel; }
	void setVel(glm::vec2 v) { vel = v; }

	~Ball();
private:
	glm::vec2 startPos;
	glm::vec2 startVel;

	glm::vec2 pos;
	glm::vec2 vel;
	float radius;
};

