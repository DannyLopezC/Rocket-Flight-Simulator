#include "PrimitiveFactory.h"
#include <vector>
#include <cmath>

PrimitiveFactory::PrimitiveFactory()
{
}

Mesh* PrimitiveFactory::createCircle(float radius, int segments)
{
    if (segments < 3) segments = 3;
    if (radius <= 0.0f) radius = 0.0001f;

    const double pi = 3.14159265358979;
    const int vertexCount = segments + 2;
    const int floatCount = vertexCount * 8;
    const int indexCount = segments * 3;

    std::vector<GLfloat> vertices;
    std::vector<unsigned int> indices;
    vertices.reserve(floatCount);
    indices.reserve(indexCount);

    // center
    vertices.push_back(0.0f); // x
    vertices.push_back(0.0f); // y
    vertices.push_back(0.0f); // z
    vertices.push_back(0.5f); // u
    vertices.push_back(0.5f); // v
    vertices.push_back(0.0f); // nx
    vertices.push_back(0.0f); // ny
    vertices.push_back(1.0f); // nz

    for (size_t i = 0; i <= segments; i++)
    {
        float angle = 2 * pi * (float)i / (float)segments;
        float x = radius * std::cos(angle);
        float y = radius * std::sin(angle);
        
        float u = (x / radius + 1.0f) * 0.5f;
        float v = (y / radius + 1.0f) * 0.5f;

        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(0.0f);
        vertices.push_back(u);
        vertices.push_back(v);
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);
        vertices.push_back(1.0f);
    }

    for (size_t i = 1; i <= segments; i++)
    {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i+1);
    }

    Mesh* circle = new Mesh();
    circle->createMesh(vertices.data(), indices.data(), floatCount, indexCount);

	return circle;
}

Mesh* PrimitiveFactory::createQuad(float width, float height)
{
    float w = width * 0.5f;
    float h = height * 0.5f;

    unsigned int quadIndices[] = { 0, 2, 1, 1, 2, 3 };
    GLfloat quadVertices[] = {
        -w, -h, 0.0f,   0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
         w, -h, 0.0f,   1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
        -w,  h, 0.0f,   0.0f, 1.0f,  0.0f, 0.0f, 1.0f,
         w,  h, 0.0f,   1.0f, 1.0f,  0.0f, 0.0f, 1.0f
    };

    Mesh* quad = new Mesh();
    quad->createMesh(quadVertices, quadIndices, 32, 6);

	return quad;
}

Mesh* PrimitiveFactory::createLine(float lenght)
{
    float l = lenght * 0.5f;

    unsigned int lineIndices[] = { 0, 1 };
    GLfloat lineVertices[] = {
        -l, 0.0f, 0.0f,   0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
         l, 0.0f, 0.0f,   1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
    };

    Mesh* line = new Mesh();
    line->createMesh(lineVertices, lineIndices, 16, 2);

    return line;
}

PrimitiveFactory::~PrimitiveFactory()
{
}
