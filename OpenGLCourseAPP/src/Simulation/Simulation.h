#pragma once
#include "glm/glm.hpp"
#include "Simulation/Ball.h"
#include "Simulation/SimulationConfig.h"
#include "Simulation/Rocket2D.h"

class Simulation
{
public:
	Simulation();

	void update(float dt);
	void restart();

	float getRocketWidth() const { return rocket.getWidth(); }
	float getRocketHeight() const { return rocket.getHeight(); }
	float getRocketAngle() const { return rocket.getAngle(); }
	float getRocketSpeed() const { return glm::length(rocket.getVel()); }
	float getMaxHeight() const { return maxHeight; }

	glm::vec2 getRocketPos() const { return rocket.getPos(); }
	glm::vec2 getRocketVel() const { return rocket.getVel(); }
	glm::vec2 getRocketAcc() const { return rocket.getAcc(); }

	float getWallX() const { return config.wallX; }
	float getFloorOffset() const { return config.floorOffset; }

	float setStartAngle(float newAngle) { config.angle = newAngle; }
	float setStartThrust(float newThrust) { config.thrust = newThrust; }
	float setStartBurnTime(float newBurnTime) { config.burnTime = newBurnTime; }
	float setStartDragCoefficient(float newDragCoefficient) { config.dragCoefficient = newDragCoefficient; }
	float setStartMass(float newMass) { config.mass = newMass; }
	float setStartRestitution(float newRestitution) { config.restitution = newRestitution; }

	~Simulation();
private:

	void bounceOnFloor();
	void bounceOnWall();
	void stopMotionOnFloorImpact();

	void applyMotion(glm::vec2 acc, float dt);
	glm::vec2 calcThrustVec();

	Rocket2D rocket;
	SimulationConfig config;

	float PI = 3.14159265358979f;
	bool simulationEnded = false;
	bool hasBounced = false;

	float maxHeight;
	float range;
	float flightTime;
};

