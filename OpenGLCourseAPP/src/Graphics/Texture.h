#pragma once
#include <GL/glew.h>
#include "../Core/stb_image.h"
#include <filesystem>

class Texture
{
public:
	Texture();
	Texture(const char* fileLocation);

	bool loadTexture();
	bool loadTextureA();

	void useTexture();
	void clearTexture();
	void createFromData(unsigned char* data, int width, int height, GLenum format);

	~Texture();

private:

	GLuint textureId;
	int width, height, bitDepth;

	const char* fileLocation;
};

