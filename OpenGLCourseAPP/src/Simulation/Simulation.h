#pragma once
#include "glm/glm.hpp"
#include "Simulation/Ball.h"
#include "Simulation/SimulationConfig.h"

class Simulation
{
public:
	Simulation();

	void update(float dt);
	void restart();

	float getBallRadius() const { return ball.getRadius(); }
	glm::vec2 getBallPos() const { return ball.getPos(); }
	glm::vec2 getBallVel() const { return ball.getVel(); }

	float getWallX() const { return config.wallX; }
	float getFloorOffset() const { return config.floorOffset; }

	~Simulation();
private:

	void bounceOnFloor();
	void bounceOnWall();

	Ball ball;
	SimulationConfig config;
};

