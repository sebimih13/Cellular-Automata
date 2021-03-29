#include "Button.h"
#include "ResourceManager.h"

#include <iostream>

Button::Button(glm::vec2 position, glm::vec2 size, glm::vec3 color, std::string text)
	: Position(position), Size(size), Text(text),
	  MainColor(color), SecondColor(color),
	  MouseInButtonRange(false), LeftMousePressed(false)
{
	InitRenderData();
}

void Button::ProcessInput(double xpos, double ypos)
{
	if (xpos >= Position.x && xpos <= Position.x + Size.x && ypos >= Position.y && ypos <= Position.y + Size.y)
	{
		MouseInButtonRange = true;
		MainColor = glm::vec3(SecondColor.x - 0.2f, SecondColor.y, SecondColor.z);
	}
	else
	{
		MouseInButtonRange = false;
		MainColor = SecondColor;
	}
}

bool Button::IsClicked()
{
	if (MouseInButtonRange && LeftMousePressed)
	{
		LeftMousePressed = false;
		return true;
	}
	return false;
}

void Button::Render(TextRenderer* TextRender, glm::vec2 textOffset)
{
	ResourceManager::GetShader("line").Use();

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(Position, 1.0f));
	model = glm::scale(model, glm::vec3(Size, 1.0f));

	ResourceManager::GetShader("line").SetMatrix4f("model", model);
	ResourceManager::GetShader("line").SetVector3f("color", MainColor);

	glBindVertexArray(SquareVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	TextRender->RenderText(Text, Position.x + textOffset.x, Position.y + textOffset.y, 1.0f);
}

void Button::SetLeftMouse(bool press)
{
	if (press && MouseInButtonRange)
		LeftMousePressed = true;
	else
		LeftMousePressed = false;
}

void Button::InitRenderData()
{
	GLuint SquareVBO;
	glGenVertexArrays(1, &SquareVAO);
	glGenBuffers(1, &SquareVBO);

	float vertices[] = {
		// pos
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,

		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f
	};

	glBindBuffer(GL_ARRAY_BUFFER, SquareVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(SquareVAO);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

