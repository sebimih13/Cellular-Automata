#include "Animation.h"
#include "ResourceManager.h"

#include <algorithm>

#define X first
#define Y second

AnimationManager::AnimationManager(float squareSize, int tableX, int tableY)
	: SquareSize(squareSize), Speed(50.0f),
	  TableX(tableX), TableY(tableY)
{
	InitRenderData();
}

void AnimationManager::InitRenderData()
{
	float SquareVertices[] = {
		0.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f
	};

	GLuint SquareVBO;
	glGenVertexArrays(1, &SquareVAO);
	glGenBuffers(1, &SquareVBO);

	glBindBuffer(GL_ARRAY_BUFFER, SquareVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(SquareVertices), SquareVertices, GL_STATIC_DRAW);

	glBindVertexArray(SquareVAO);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void AnimationManager::Draw(float deltaTime)
{
	for (Animation& sq : BlockAnims)
	{
		sq.scale = std::min(SquareSize - 2.0f, sq.scale + Speed * deltaTime);
		DrawSquare(sq);
	}
}

void AnimationManager::DrawSquare(Animation& sq)
{
	float StartX = TableX + (float)sq.position.Y * SquareSize;
	float StartY = TableY + (float)sq.position.X * SquareSize;

	ResourceManager::GetShader("line").Use();
	ResourceManager::GetShader("line").SetVector3f("color", glm::vec3(0.0f, 0.0f, 0.0f));
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(StartX + (SquareSize - sq.scale) / 2.0f, StartY + (SquareSize - sq.scale) / 2.0f, 0.0f));
	model = glm::scale(model, glm::vec3(sq.scale, sq.scale, 0.0f));
	ResourceManager::GetShader("line").SetMatrix4f("model", model);

	glBindVertexArray(SquareVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void AnimationManager::AddBlock(coordinates sq)
{
	if (FindBlock(sq) == -1)

		BlockAnims.push_back(Animation(sq));
}

int AnimationManager::FindBlock(coordinates& sq)
{
	for (int index = 0; index < BlockAnims.size(); index++)
		if (BlockAnims[index].position.X == sq.X && BlockAnims[index].position.Y == sq.Y)
			return index;
	return -1;
}

void AnimationManager::DeleteBlock(coordinates sq)
{
	int element = FindBlock(sq);
	if (element != -1)
		BlockAnims.erase(BlockAnims.begin() + element);
}

void AnimationManager::Reset()
{
	BlockAnims.clear();
}

void AnimationManager::SetTablePosition(int x, int y)
{
	TableX = x;
	TableY = y;
}

void AnimationManager::SetTableSquareSize(float size)
{
	SquareSize = size;
}

