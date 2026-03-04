#include "Ball.h"

Ball::Ball(glm::vec2 startPos, glm::vec2 startVel, float r)
{
}

void Ball::update(float dt, float gravity)
{
	vel.y -= gravity * dt;
	pos += vel * dt;
}

void Ball::bounceOnFloor(float floorY, float restitution)
{
	if (pos.y - radius <= floorY)
	{
		pos.y = floorY + radius;
		vel.y = -vel.y * restitution;

		if (std::abs(vel.y) < 0.05f) vel.y = 0.0f;
	}
}

Ball::~Ball()
{
}
