#include "Ball.h"

Ball::Ball(glm::vec2 startPos, glm::vec2 startVel, float r) : startPos(startPos), startVel(startVel)
{
	restart();
	radius = r;
}

void Ball::restart()
{
	pos = startPos;
	vel = startVel;
}

Ball::~Ball()
{
}