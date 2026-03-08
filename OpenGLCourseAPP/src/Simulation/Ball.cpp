#include "Ball.h"

Ball::Ball(glm::vec2 startPos, glm::vec2 startVel, float r)
{
	pos = startPos;
	vel = startVel;
	radius = r;
}

void Ball::restart(glm::vec2 pos, glm::vec2 vel)
{
	this->pos = pos;
	this->vel = vel;
}

Ball::~Ball()
{
}