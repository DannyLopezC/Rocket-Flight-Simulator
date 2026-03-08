#include "Simulation.h"

Simulation::Simulation()
{
	ball = Ball(config.throwerPos, config.v0, 0.2f);
}

void Simulation::update(float dt)
{
	ball.setYVel(ball.getVel().y - (config.gravity * dt));
	ball.setPos(ball.getPos() + ball.getVel() * dt);

	bounceOnFloor();
	bounceOnWall();
}

void Simulation::bounceOnFloor()
{
	if (ball.getPos().y - ball.getRadius() <= config.floorTopY)
	{
		ball.setYPos(config.floorTopY + ball.getRadius());
		ball.setYVel(-ball.getVel().y * config.restitution);

		if (std::abs(ball.getVel().y) < 0.05f) ball.setYVel(0.0f);

		std::cout << ball.getPos().x << std::endl;
	}
}

void Simulation::bounceOnWall()
{
	if (ball.getPos().x + ball.getRadius() >= config.wallX) {
		ball.setXPos(config.wallX - ball.getRadius());
		ball.setXVel(-ball.getVel().x * config.restitution);

		if (std::abs(ball.getVel().x) < 0.05f) ball.setXVel(0.0f);
	}
}

void Simulation::restart() {
	ball.restart();
}

Simulation::~Simulation()
{
}
