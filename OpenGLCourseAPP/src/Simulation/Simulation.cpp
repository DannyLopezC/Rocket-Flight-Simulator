#include "Simulation.h"
#include <iostream>
#include <cmath>
#include <algorithm>

Simulation::Simulation()
{
	//ball = Ball(config.throwerPos, config.v0, 0.2f);
	rocket = Rocket2D(config.startPos, config.startVel, config.startAcc, config.angle,
		config.mass, config.thrust, config.burnTime, 0.4f, 0.8f);

	maxHeight = 0;
	range = 0; 
	flightTime = 0;
}

void Simulation::update(float dt)
{
	if (simulationEnded) return;
	flightTime += dt;
	if (rocket.getPos().y > maxHeight) maxHeight = rocket.getPos().y;
	glm::vec2 gravityVec = rocket.getMass() * glm::vec2(0, (-config.gravity));
	glm::vec2 thrustVec = calcThrustVec();
	glm::vec2 drag = -config.dragCoefficient * rocket.getVel(); // air resistance
	glm::vec2 totalForce = thrustVec + gravityVec + drag;

	applyMotion(totalForce/ rocket.getMass(), dt);

	float burnTime = std::max(0.0f, rocket.getBurnTime() - dt);
	rocket.setBurnTime(burnTime);

	bounceOnFloor();
	//bounceOnWall();
}

void Simulation::bounceOnFloor()
{
	if (rocket.getPos().y - (rocket.getHeight() / 2) <= config.floorTopY)
	{
		if (!hasBounced) range = rocket.getPos().x - config.startPos.x;
		hasBounced = true;
		rocket.setYPos(config.floorTopY + rocket.getHeight() / 2);
		rocket.setYVel(-rocket.getVel().y * config.restitution);

		if (std::abs(rocket.getVel().y) < 0.05f) {
			simulationEnded = true;
			std::cout << "Rocket Flight Simulator" << std::endl;
			std::cout << "Max Height: " << maxHeight << std::endl;
			std::cout << "Range: " << range << std::endl;
			std::cout << "Flight Time: " << flightTime << std::endl;

			//rocket.setVel(glm::vec2(0, 0));
			rocket.setAcc(glm::vec2(0, 0));
		}
	}
}

void Simulation::bounceOnWall()
{
	if (rocket.getPos().x + (rocket.getWidth()/2) >= config.wallX) {
		rocket.setXPos(config.wallX - (rocket.getWidth() / 2));
		rocket.setXVel(-rocket.getVel().x * config.restitution);

		if (std::abs(rocket.getVel().x) < 0.05f) rocket.setXVel(0.0f);
	}
}

void Simulation::stopMotionOnFloorImpact()
{
	if (rocket.getPos().y - (rocket.getHeight() / 2) <= config.floorTopY)
	{
		rocket.setVel(glm::vec2(0, 0));
		rocket.setAcc(glm::vec2(0, 0));

		std::cout << rocket.getPos().x << std::endl;
		simulationEnded = true;
		maxHeight = maxHeight - config.startPos.x;
	}
}

void Simulation::applyMotion(glm::vec2 acc, float dt)
{
	rocket.setAcc(acc);
	rocket.setVel(rocket.getVel() + rocket.getAcc() * dt);
	rocket.setPos(rocket.getPos() + rocket.getVel() * dt);
}

glm::vec2 Simulation::calcThrustVec()
{
	glm::vec2 thrustVec = { 0, 0 };

	if (rocket.getBurnTime() > 0) {
		float angleRad = rocket.getAngle() * (PI / 180);
		float x = cos(angleRad);
		float y = sin(angleRad);
		thrustVec = rocket.getThrust() * glm::vec2(x, y);
	}

	return thrustVec;
}

void Simulation::restart() {
	rocket.restart(config.startPos, config.startVel, config.burnTime);
	simulationEnded = false;
	hasBounced = false;
	maxHeight = 0;
	range = 0;
	flightTime = 0;
}

Simulation::~Simulation()
{
}
