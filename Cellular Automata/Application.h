#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Camera.h"

class Application
{
public:
	// constructor/destructor
	Application(unsigned int width, unsigned int height);
	~Application();

	// game loop
	void Init();
	void Update(float deltaTime, GLFWwindow* window);
	void Draw();
	void SetMousePosition(double xpos, double ypos);
	void SetLeftMouse(bool press);
	void SetRightMouse(bool press);

private:
	// state
	unsigned int Width, Height;

	// todo
	GLuint LineVAO;
	float SquareSize = 30.0f;

	// components
	Camera* camera;
	bool firstMouse = true;
	float lastX = (float)Width / 2.0f;
	float lastY = (float)Height / 2.0f;
};

