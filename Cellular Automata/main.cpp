#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <cmath>
#include <vector>

#include "ResourceManager.h"
#include "Animation.h"
#include "TextRenderer.h"
#include "Button.h"

// callback
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_cursor_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

// utility functions
void Init();
void DrawTable();
void DrawInterface(GLFWwindow* window);
void processInput(GLFWwindow* window);
void ProcessNextGeneration();

// window settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;

// time variables
double deltaTime = 0.0f;		// time between current frame and last frame
double lastFrame = 0.0f;		// time of last frame

// table settings
const int MAX_ZOOM = 50;
const int MIN_ZOOM = 10;
const int MAX_TABLE_WIDTH = 200;
const int MAX_TABLE_HEIGHT = 200;

int TABLE_WIDTH = 100;
int TABLE_HEIGHT = 100;
int SquareSize = 10;

// table state
enum ETableState
{
	TABLE_INPUT,
	TABLE_DRAW,
	TABLE_PLAY,
	TABLE_PAUSE
} TableState;

// table coordinates
float TableUpX = 0;
float TableUpY = 0;

// mouse
bool IsLeftMousePressed;
bool IsRightMousePressed;
bool IsLeftCtrlPressed;
double LastX, LastY;

// animation manager
AnimationManager* Animations;

// text renderer
TextRenderer* RenderText;

// cursor settings
GLFWcursor* HandCursor;
GLFWcursor* TextCursor;

const float NextGenerationAnimation = 1.5f;
float TimerAnimation = 0.0f;

// user input
bool TableWidthSelected;
bool TableHeightSelected;
Button* BeginButton;

// game of life
std::vector<std::vector<bool>> TableMatrix, AuxTable;

int main()
{
	// glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_RESIZABLE, false);

	// glfw window creation
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Cellular Automata", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_cursor_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global OpenGL state
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// cursor shape
	HandCursor = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
	TextCursor = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);


	Init();

	TableState = ETableState::TABLE_INPUT;

	Animations = new AnimationManager((float)SquareSize, (int)TableUpX, (int)TableUpY);
	RenderText = new TextRenderer(SCR_WIDTH, SCR_HEIGHT);
	BeginButton = new Button(glm::vec2(SCR_WIDTH / 2.0f - 80.0f, SCR_HEIGHT / 2.0f - 80.0f), glm::vec2(150.0f, 50.0f), glm::vec3(0.5f, 0.5f, 0.5f), "Begin");

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		// time
		double currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;


		// input
		processInput(window);


		// rendering commands here
		glClearColor(0.83137f, 0.82353f, 0.83137f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);


		if (TableState == ETableState::TABLE_INPUT)
		{
			DrawInterface(window);
		}
		else
		{
			if (TableState == ETableState::TABLE_PLAY)
			{
				if (TimerAnimation >= NextGenerationAnimation)
				{
					ProcessNextGeneration();
					TimerAnimation = 0.0f;
				}
				else
				{
					TimerAnimation += (float)deltaTime;
				}
			}

			DrawTable();
			Animations->Draw((float)deltaTime);
		}


		// check and call events and swap the buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// delete pointers
	delete Animations;
	delete RenderText;
	delete BeginButton;

	// glfw: terminate, clearing all previously allocated GLFW resources
	glfwTerminate();
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//														Callbacks
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (TableState == ETableState::TABLE_INPUT)
	{
		if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS)
		{
			if (TableWidthSelected)
				TABLE_WIDTH /= 10;
			else if (TableHeightSelected)
				TABLE_HEIGHT /= 10;
		}

		for (int i = 0; i < 19; i++)
		{
			if ((key == GLFW_KEY_0 + i || key == GLFW_KEY_KP_0 + i) && action == GLFW_PRESS)
			{
				if (TableWidthSelected)
					TABLE_WIDTH = TABLE_WIDTH * 10 + i;
				else if (TableHeightSelected)
					TABLE_HEIGHT = TABLE_HEIGHT * 10 + i;
			}

			if (TABLE_WIDTH > MAX_TABLE_WIDTH)
				TABLE_WIDTH /= 10;
			if (TABLE_HEIGHT > MAX_TABLE_HEIGHT)
				TABLE_HEIGHT /= 10;
		}
	}
	else
	{
		if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS)
		{
			TableState = ETableState::TABLE_DRAW;
			TimerAnimation = 0.0f;
			Animations->Reset();
			TableMatrix.assign(TABLE_HEIGHT, std::vector<bool>(TABLE_WIDTH, false));
			AuxTable.assign(TABLE_HEIGHT, std::vector<bool>(TABLE_WIDTH, false));
		}

		if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		{
			if (TableState == ETableState::TABLE_DRAW)
				TableState = ETableState::TABLE_PLAY;
			else if (TableState == ETableState::TABLE_PLAY)
				TableState = ETableState::TABLE_PAUSE;
			else if (TableState == ETableState::TABLE_PAUSE)
				TableState = ETableState::TABLE_PLAY;
		}

		if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS)
		{
			glfwGetCursorPos(window, &LastX, &LastY);
			glfwSetCursor(window, HandCursor);
			IsLeftCtrlPressed = true;
		}
		else if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_RELEASE)
		{
			IsLeftCtrlPressed = false;
			glfwSetCursor(window, NULL);
		}
	}
}

void processInput(GLFWwindow* window)
{
	if (TableState == ETableState::TABLE_DRAW)
	{
		// Check if a square is selected
		glfwGetCursorPos(window, &LastX, &LastY);

		int TableDownX = (int)TableUpX + SquareSize * TABLE_WIDTH;
		int TableDownY = (int)TableUpY + SquareSize * TABLE_HEIGHT;

		if ((int)TableUpX < LastX && LastX < TableDownX && (int)TableUpY < LastY && LastY < TableDownY)
		{
			int SquareRow = ((int)LastY - (int)TableUpY) / SquareSize;
			int SquareColumn = ((int)LastX - (int)TableUpX) / SquareSize;

			if (IsRightMousePressed)
			{
				Animations->DeleteBlock({ SquareRow, SquareColumn });
				TableMatrix[SquareRow][SquareColumn] = false;
			}
			else if (IsLeftMousePressed)
			{
				Animations->AddBlock({ SquareRow, SquareColumn });
				TableMatrix[SquareRow][SquareColumn] = true;
			}
		}
	}
}

void mouse_cursor_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (TableState == ETableState::TABLE_INPUT)
	{
		BeginButton->ProcessInput(xpos, ypos);
	}
	else
	{
		if (IsLeftCtrlPressed)
		{
			double xoffset = xpos - LastX;
			double yoffset = LastY - ypos;
			LastX = xpos;
			LastY = ypos;

			TableUpX += (float)xoffset;
			TableUpY -= (float)yoffset;

			Animations->SetTablePosition((int)TableUpX, (int)TableUpY);
		}
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (TableState == ETableState::TABLE_INPUT)
		return;

	int LastSquareSize = SquareSize;
	SquareSize += (int)yoffset;

	if (SquareSize < MIN_ZOOM)
		SquareSize = MIN_ZOOM;

	if (SquareSize > MAX_ZOOM)
		SquareSize = MAX_ZOOM;

	if (LastSquareSize == SquareSize)
		return;

	Animations->SetTableSquareSize((float)SquareSize);
	
	glfwGetCursorPos(window, &LastX, &LastY);

	float DiffX = (float)LastX - TableUpX;
	float DiffY = (float)LastY - TableUpY;

	float LastSquareX = DiffX / (float)LastSquareSize;
	float LastSquareY = DiffY / (float)LastSquareSize;
	
	// DiffX = ? a.i.:				DiffX / SquareSize == LastSquareX
	//							   (LastX - TableUpX) / SquareSize == LastSquareX
	//							   (LastX - TableUpX) / SquareSize == LastSquareX
	//							    LastX - TableUpX == LastSquareX * SquareSize | * (-1)
	//							    TableUpX == LastX - LastSquareX * SquareSize

	TableUpX = (float)LastX - LastSquareX * (float)SquareSize;
	TableUpY = (float)LastY - LastSquareY * (float)SquareSize;

	Animations->SetTablePosition((int)TableUpX, (int)TableUpY);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (TableState == ETableState::TABLE_INPUT)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
			BeginButton->SetLeftMouse(true);
		else
			BeginButton->SetLeftMouse(false);
	}
	else
	{
		if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
			IsRightMousePressed = true;
		else
			IsRightMousePressed = false;

		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
			IsLeftMousePressed = true;
		else
			IsLeftMousePressed = false;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//														Utility functions
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GLuint LineVAO;
void Init()
{
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//													Configure VAO/VBO
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// LINE
	float LineVertices[] = {
		0.0f, 0.0f,			// row left
		1.0f, 0.0f,			// row right
	};

	GLuint LineVBO;
	glGenVertexArrays(1, &LineVAO);
	glGenBuffers(1, &LineVBO);

	glBindBuffer(GL_ARRAY_BUFFER, LineVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(LineVertices), LineVertices, GL_STATIC_DRAW);

	glBindVertexArray(LineVAO);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);


	// load shaders
	ResourceManager::LoadShader("shaders/line.vert", "shaders/line.frag", nullptr, "line");

	// configure shader
	glm::mat4 projection = glm::ortho(0.0f, (float)SCR_WIDTH, (float)SCR_HEIGHT, 0.0f);

	ResourceManager::GetShader("line").Use();
	ResourceManager::GetShader("line").SetMatrix4f("projection", projection);
}

void DrawTable()
{
	glm::mat4 model = glm::mat4(1.0f);

	// draw rows and columns
	glBindVertexArray(LineVAO);
	ResourceManager::GetShader("line").Use();
	ResourceManager::GetShader("line").SetVector3f("color", glm::vec3(0.0f, 0.0f, 0.0f));

	// draw rows
	for (int y = (int)TableUpY; y <= (int)TableUpY + TABLE_HEIGHT * SquareSize; y += SquareSize)
	{
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3((int)TableUpX, y, 0.0f));
		model = glm::scale(model, glm::vec3((float)(TABLE_WIDTH * SquareSize), 0.0f, 0.0f));
		ResourceManager::GetShader("line").SetMatrix4f("model", model);

		glDrawArrays(GL_LINES, 0, 2);
	}

	// draw columns
	for (int x = (int)TableUpX; x <= (int)TableUpX + TABLE_WIDTH * SquareSize; x += SquareSize)
	{
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(x, (int)TableUpY, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3((float)(TABLE_HEIGHT * SquareSize), 0.0f, 0.0f));
		ResourceManager::GetShader("line").SetMatrix4f("model", model);

		glDrawArrays(GL_LINES, 0, 2);
	}

	glBindVertexArray(0);
}

void DrawInterface(GLFWwindow* window)
{
	RenderText->Load("fonts/Antonio-Bold.ttf", 70);
	RenderText->RenderText("GAME OF LIFE", (float)SCR_WIDTH / 2.0f - 165.0f, 100.0f, 1.0f, glm::vec3(0.0f, 0.0f, 0.0f));

	RenderText->Load("fonts/Antonio-Regular.ttf", 40);
	RenderText->RenderText("Table Width: ", (float)SCR_WIDTH / 2.0f - 130.0f, 200.0f, 1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	RenderText->RenderText(std::to_string(TABLE_WIDTH), 660.0f, 200.0f, 1.0f, glm::vec3(0.0f, 0.0f, 0.0f));

	RenderText->RenderText("Table Height: ", (float)SCR_WIDTH / 2.0f - 138.0f, 260.0f, 1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	RenderText->RenderText(std::to_string(TABLE_HEIGHT), 660.0f, 260.0f, 1.0f, glm::vec3(0.0f, 0.0f, 0.0f));

	BeginButton->Render(RenderText, glm::vec2(35.0f, 5.0f));

	RenderText->Load("fonts/Antonio-Regular.ttf", 20);
	RenderText->RenderText("Left Click = draw square", 20.0f, (float)SCR_HEIGHT - 200.0f, 1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	RenderText->RenderText("Right Click = erase square", 20.0f, (float)SCR_HEIGHT - 170.0f, 1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	RenderText->RenderText("Space = resume/pause game of life", 20.0f, (float)SCR_HEIGHT - 140.0f, 1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	RenderText->RenderText("Left CTRL = move board", 20.0f, (float)SCR_HEIGHT - 110.0f, 1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	RenderText->RenderText("Backspace = clear board", 20.0f, (float)SCR_HEIGHT - 80.0f, 1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	RenderText->RenderText("ESC = exit", 20.0f, (float)SCR_HEIGHT - 50.0f, 1.0f, glm::vec3(0.0f, 0.0f, 0.0f));


	// check user input
	glfwGetCursorPos(window, &LastX, &LastY);

	if (660.0f < LastX && LastX < 800.0f && 200.0f < LastY && LastY < 240.0f)		// table width is selected
	{
		glfwSetCursor(window, TextCursor);
		TableWidthSelected = true;
		TableHeightSelected = false;
	}
	else if (660.0f < LastX && LastX < 800.0f && 250.0f < LastY && LastY < 290.0f)	// table height is selected
	{
		glfwSetCursor(window, TextCursor);
		TableWidthSelected = false;
		TableHeightSelected = true;
	}
	else
	{
		glfwSetCursor(window, NULL);
		TableWidthSelected = false;
		TableHeightSelected = false;
	}

	if (BeginButton->IsClicked())
	{
		TableMatrix.assign(TABLE_HEIGHT, std::vector<bool>(TABLE_WIDTH, false));
		AuxTable.assign(TABLE_HEIGHT, std::vector<bool>(TABLE_WIDTH, false));
		TableState = ETableState::TABLE_DRAW;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//														Game of life
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*

	1. Any live cell with fewer than two live neighbours dies, as if by underpopulation.
	2. Any live cell with two or three live neighbours lives on to the next generation.
	3. Any live cell with more than three live neighbours dies, as if by overpopulation.
	4. Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.

*/

bool InMatrix(int x, int y)
{
	return 0 <= x && x < TABLE_HEIGHT && 0 <= y && y < TABLE_WIDTH;
}

const int DirX[] = { 0, 0, 1, 1, 1, -1, -1, -1 };
const int DirY[] = { -1, 1, -1, 0, 1, -1, 0, 1 };

void ProcessNextGeneration()
{
	Animations->Reset();

	for (int x = 0; x < TABLE_HEIGHT; x++)
	{
		for (int y = 0; y < TABLE_WIDTH; y++)
		{
			int LivingCells = 0;
			for (int i = 0; i < 8; i++)
			{
				std::pair<int, int> neighbour = { x + DirX[i], y + DirY[i] };
				if (InMatrix(neighbour.first, neighbour.second) && TableMatrix[neighbour.first][neighbour.second])
					LivingCells++;
			}

			if (TableMatrix[x][y])
			{
				if (2 <= LivingCells && LivingCells <= 3)
					AuxTable[x][y] = true;
				else
					AuxTable[x][y] = false;
			}
			else if (LivingCells == 3)
			{
				AuxTable[x][y] = true;
			}

			if (AuxTable[x][y])
				Animations->AddBlock({ x, y });
		}
	}

	TableMatrix = AuxTable;
}

