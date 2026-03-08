#pragma once
#include <glm/glm.hpp>

struct SimulationConfig
{
	float gravity = 10;

	glm::vec2 throwerPos = { 0.0f, 2.0f };
	glm::vec2 v0 = { 10.0f, 10.0f };

	float floorTopY = 0.0f;
	float floorOffset = 9.0;
	float wallX = 4.0f;

	float restitution = 1.0;
};