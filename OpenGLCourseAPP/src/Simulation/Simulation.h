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

	// rocket values
	float getRocketWidth() const { return rocket.getWidth(); }
	float getRocketHeight() const { return rocket.getHeight(); }
	float getRocketAngle() const { return rocket.getAngle(); }
	float getRocketSpeed() const { return glm::length(rocket.getVel()); }
	glm::vec2 getRocketPos() const { return rocket.getPos(); }
	glm::vec2 getRocketVel() const { return rocket.getVel(); }
	glm::vec2 getRocketAcc() const { return rocket.getAcc(); }

	// parameters to show
	float getMaxHeight() const { return maxHeight; }
	float getRange() const { return range; }
	float getFlightTime() const { return flightTime; }

	// starting values
	void setGravity(float v) { config.gravity = v; }
	void setAngle(float v) { config.angle = v; rocket.setAngle(v); }
	void setMass(float v) { config.mass = v; rocket.setMass(v); }
	void setThrust(float v) { config.thrust = v; rocket.setThrust(v); }
	void setBurnTime(float v) { config.burnTime = v; rocket.setBurnTime(v); }
	void setFloorTopY(float v) { config.floorTopY = v; }
	void setFloorOffset(float v) { config.floorOffset = v; }
	void setWallX(float v) { config.wallX = v; }
	void setRestitution(float v) { config.restitution = v; }
	void setDragCoefficient(float v) { config.dragCoefficient = v; }

	float getGravity() const { return config.gravity; }
	float getAngle() const { return config.angle; }
	float getMass() const { return config.mass; }
	float getThrust() const { return config.thrust; }
	float getBurnTime() const { return config.burnTime; }
	float getFloorTopY() const { return config.floorTopY; }
	float getFloorOffset() const { return config.floorOffset; }
	float getWallX() const { return config.wallX; }
	float getRestitution() const { return config.restitution; }
	float getDragCoefficient() const { return config.dragCoefficient; }

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
	float burnTime;
};

