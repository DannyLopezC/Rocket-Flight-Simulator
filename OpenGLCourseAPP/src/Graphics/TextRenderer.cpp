#include "TextRenderer.h"

TextRenderer::TextRenderer()
{
	textShader = nullptr;
}

void TextRenderer::init(unsigned int screenWidth, unsigned int screenHeight, Shader* shader)
{
	textShader = shader;

	glm::mat4 projection = glm::ortho(
		0.0f, static_cast<float>(screenWidth),
		0.0f, static_cast<float>(screenHeight)
	);

	textShader->useShader();

	GLint projectionLoc = glGetUniformLocation(textShader->getShaderId(), "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

bool TextRenderer::loadFont(std::string path, float fontSize)
{
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		std::cerr << "ERROR::FREETYPE: Could not init freetype library" << std::endl;
		return false;
	}

	FT_Face face;
	if (FT_New_Face(ft, path.c_str(), 0, &face))
	{
		std::cerr << "ERROR::FREETYPE: Failed to load font: " << path << std::endl;
		FT_Done_FreeType(ft);
		return false;
	}

	FT_Set_Pixel_Sizes(face, 0, fontSize);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	characters.clear();

	for (unsigned char c = 0; c < 128; c++)
	{
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cerr << "ERROR::FREETYPE: Failed to load glyph for char: " << c << std::endl;
			continue;
		}

		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width,
			face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			static_cast<unsigned int>(face->glyph->advance.x)
		};

		characters.insert(std::pair<char, Character>(c, character));
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	return true;
}

void TextRenderer::renderText(std::string text, float x, float y, float scale, const glm::vec3& color)
{
	if (!textShader)return;

	textShader->useShader();

	GLint textColorLoc = glGetUniformLocation(textShader->getShaderId(), "textColor");
	glUniform3f(textColorLoc, color.x, color.y, color.z);

	glActiveTexture(GL_TEXTURE0);
	GLint textSamplerLoc = glGetUniformLocation(textShader->getShaderId(), "text");
	glUniform1i(textSamplerLoc, 0);

	glBindVertexArray(VAO);

	for (char c : text)
	{
		auto it = characters.find(c);
		if (it == characters.end()) continue;

		Character ch = it->second;

		float xpos = x + ch.bearing.x * scale;
		float ypos = y - (ch.size.y - ch.bearing.y) * scale;

		float w = ch.size.x * scale;
		float h = ch.size.y * scale;

		float vertices[6][4] = {
			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },

			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 0.0f }
		};

		glBindTexture(GL_TEXTURE_2D, ch.textureID);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		// advance está en 1/64 pixels
		x += (ch.advance >> 6) * scale;
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

TextRenderer::~TextRenderer()
{
	textShader = nullptr;

	for (auto& pair : characters)
	{
		glDeleteTextures(1, &pair.second.textureID);
	}

	if (VBO != 0) glDeleteBuffers(1, &VBO);
	if (VAO != 0) glDeleteVertexArrays(1, &VAO);
}
