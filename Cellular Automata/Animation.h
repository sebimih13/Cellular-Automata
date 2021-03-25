#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <iostream>
#include <vector>

typedef std::pair<unsigned int, unsigned int> coordinates;

struct Animation
{
	Animation(coordinates pos)
	{
		position = pos;
	}

	coordinates position;
	float scale = 1.0f;
};

class AnimationManager
{
public:
	// constructor
	AnimationManager(float squareSize, int tableX, int tableY);

	// funtions for adding/removing squares
	void AddBlock(coordinates sq);
	int FindBlock(coordinates& sq);
	void DeleteBlock(coordinates sq);

	// table funtions
	void SetTablePosition(int x, int y);
	void SetTableSquareSize(float size);

	// draw
	void Draw(float deltaTime);

	// delete all squares
	void Reset();

private:
	// store all squares that must be animated
	std::vector<Animation> BlockAnims;
	float SquareSize;
	float Speed;
	int TableX, TableY;

	// initialize render data
	void InitRenderData();

	// render data
	GLuint SquareVAO;

	// draw
	void DrawSquare(Animation& sq);
};

