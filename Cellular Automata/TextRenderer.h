#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <map>

#include "shader.h"

struct Character
{
	GLuint TextureID;		// ID handle of the glyph texture
	glm::ivec2 Size;		// size of glyph
	glm::ivec2 Bearing;		// offset from baseline to left/top of glyph
	unsigned int Advance;	// horizontal offset to advance to next glyph
};

class TextRenderer
{
public:
	// constructor
	TextRenderer(unsigned int width, unsigned int height);

	// pre-compile a list of characters from given font
	void Load(std::string font, unsigned int fontSize);

	// render aa string of text using the precompiled list of characters
	void RenderText(std::string text, float x, float y, float scale, glm::vec3 color = glm::vec3(1.0f));

private:
	// render state
	GLuint VAO, VBO;

	// list of precompiled Characters
	std::map<char, Character> Characters;
};

#endif
