#pragma once
#include "Mesh.h"

class PrimitiveFactory
{
public:
	static Mesh* createCircle(float radius, int segments);
	static Mesh* createQuad(float width, float height);
	static Mesh* createUIQuad(float width, float height);
	static Mesh* createLine();
	static Mesh* createArrowHead();

private:

};

