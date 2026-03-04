#pragma once
#include "Mesh.h"

class PrimitiveFactory
{
public:
	PrimitiveFactory();

	Mesh* createCircle(float radius, int segments);
	Mesh* createQuad(float width, float height);
	Mesh* createLine(float lenght);

	~PrimitiveFactory();

private:

};

