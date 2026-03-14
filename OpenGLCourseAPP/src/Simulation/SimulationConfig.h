#pragma once
#include <glm/glm.hpp>

struct SimulationConfig
{
	float gravity = 10;

	glm::vec2 startPos = { -15.0f, 0.5f };
	glm::vec2 startVel = { 0.0f, 0.0f };
	glm::vec2 startAcc = { 0.0f, 0.0f };

	float angle = 85.0f;     // degrees
	float mass = 1.0f;       // kg
	float thrust = 12.0f;    // motor force
	float burnTime = 3.0f;   // push seconds
	
	float floorTopY = -0.5f; // actual physic floor
	float floorOffset = 9.0; // this is only visual
	float wallX = 4.0f;

	float restitution = 0.5f;
	float dragCoefficient = 0.0f;
};