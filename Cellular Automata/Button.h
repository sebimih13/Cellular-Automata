#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "TextRenderer.h"

#include <string>

class Button
{
public:
	// constructor
	Button(glm::vec2 position, glm::vec2 size, glm::vec3 color, std::string text);

	void ProcessInput(double xpos, double ypos);
	void SetLeftMouse(bool press);

	void Render(TextRenderer* TextRender, glm::vec2 textOffset);

	bool IsClicked();

private:
	// button state
	std::string Text;
	glm::vec2 Position, Size;
	glm::vec3 MainColor, SecondColor;

	bool MouseInButtonRange;
	bool LeftMousePressed;

	// render data
	GLuint SquareVAO;
	void InitRenderData();
};

