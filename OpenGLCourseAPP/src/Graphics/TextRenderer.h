#pragma once
#include <map>
#include <string>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "Graphics/Shader.h"

struct Character
{
	unsigned int textureID;
	glm::ivec2 size;
	glm::ivec2 bearing;
	unsigned int advance;
};

class TextRenderer
{
public:
	TextRenderer();

	void init(unsigned int screenWidth, unsigned int screenHeight, Shader* shader);
	bool loadFont(std::string path, float fontSize);
	void renderText(std::string text, float x, float y, float scale, const glm::vec3& color);

	~TextRenderer();
private:
	std::map<char, Character> characters;

	GLuint VAO = 0, VBO = 0;
	
	Shader* textShader;
};

