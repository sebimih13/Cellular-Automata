#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <cmath>
#include <vector>

#include "ResourceManager.h"
#include "Animation.h"

// callback
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_cursor_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

// utility functions
void Init();
void DrawTable();

// window settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;

// time variables
double deltaTime = 0.0f;		// time between current frame and last frame
double lastFrame = 0.0f;		// time of last frame

// table settings
const int TABLE_WIDTH = 10;
const int TABLE_HEIGHT = 10;
const int MAX_ZOOM = 50;
const int MIN_ZOOM = 10;
int SquareSize = 10;

// table state
enum ETableState
{
	TABLE_DRAW,
	TABLE_VISUALIZE,
	TABLE_PAUSE
} TableState;

// cursor settings
GLFWcursor* HandCursor;

// table coordinates
int TableUpX = 0;
int TableUpY = 0;

// mouse
bool IsLeftMousePressed;
bool IsRightMousePressed;
bool IsLeftCtrlPressed;
double LastX, LastY;

// animation manager
AnimationManager* Animations;

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


	Init();

	TableState = ETableState::TABLE_DRAW;

	Animations = new AnimationManager((float)SquareSize, TableUpX, TableUpY);
	

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

		DrawTable();
		Animations->Draw((float)deltaTime);


		// check and call events and swap the buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// delete pointers
	delete Animations;

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

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		glfwGetCursorPos(window, &LastX, &LastY);
		glfwSetCursor(window, HandCursor);
		IsLeftCtrlPressed = true;
	}
	else
	{
		IsLeftCtrlPressed = false;
		glfwSetCursor(window, NULL);
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		if (TableState == ETableState::TABLE_DRAW)
			TableState = ETableState::TABLE_VISUALIZE;
		else if (TableState == ETableState::TABLE_VISUALIZE)
			TableState = ETableState::TABLE_PAUSE;
		else if (TableState == ETableState::TABLE_PAUSE)
			TableState = ETableState::TABLE_VISUALIZE;
	}

	
	if (TableState == ETableState::TABLE_DRAW)
	{
		// Check if a square is selected
		glfwGetCursorPos(window, &LastX, &LastY);

		int TableDownX = TableUpX + SquareSize * TABLE_WIDTH;
		int TableDownY = TableUpY + SquareSize * TABLE_HEIGHT;

		if (TableUpX < LastX && LastX < TableDownX && TableUpY < LastY && LastY < TableDownY)
		{
			int SquareRow = ((int)LastY - TableUpY) / SquareSize;
			int SquareColumn = ((int)LastX - TableUpX) / SquareSize;

			// todo
			// std::cout << "Square : " << SquareRow << ' ' << SquareColumn << '\n';

			if (IsRightMousePressed)
				Animations->DeleteBlock({ SquareRow, SquareColumn });
			else if (IsLeftMousePressed)
				Animations->AddBlock({ SquareRow, SquareColumn });
		}
	}
}

void mouse_cursor_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (IsLeftCtrlPressed)
	{
		double xoffset = xpos - LastX;
		double yoffset = LastY - ypos;
		LastX = xpos;
		LastY = ypos;

		TableUpX += (int)xoffset;
		TableUpY -= (int)yoffset;

		Animations->SetTablePosition(TableUpX, TableUpY);
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
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

	int DiffX = (int)LastX - TableUpX;
	int DiffY = (int)LastY - TableUpY;

	float LastSquareX = (float)DiffX / (float)LastSquareSize;
	float LastSquareY = (float)DiffY / (float)LastSquareSize;
	
	// TODO ->
	// DiffX = ? a.i.:				DiffX / SquareSize == LastSquareX
	//							   (LastX - TableUpX) / SquareSize == LastSquareX
	//							   (LastX - TableUpX) / SquareSize == LastSquareX
	//							    LastX - TableUpX == LastSquareX * SquareSize | * (-1)
	//							    TableUpX == LastX - LastSquareX * SquareSize

	TableUpX = (int)((float)LastX - LastSquareX * (float)SquareSize);
	TableUpY = (int)((float)LastY - LastSquareY * (float)SquareSize);

	Animations->SetTablePosition(TableUpX, TableUpY);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		IsRightMousePressed = true;
	}
	else
	{
		IsRightMousePressed = false;
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		IsLeftMousePressed = true;
	}
	else
	{
		IsLeftMousePressed = false;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//														Utility functions
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// todo : do we need a QuadVAO?
GLuint LineVAO, QuadVAO;
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

	// QUAD
	float QuadVertices[] = {
		0.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f
	};

	GLuint QuadVBO;
	glGenVertexArrays(1, &QuadVAO);
	glGenBuffers(1, &QuadVBO);

	glBindBuffer(GL_ARRAY_BUFFER, QuadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(QuadVertices), QuadVertices, GL_STATIC_DRAW);

	glBindVertexArray(QuadVAO);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//														Shaders
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
	for (int y = TableUpY; y <= TableUpY + TABLE_HEIGHT * SquareSize; y += SquareSize)
	{
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(TableUpX, y, 0.0f));
		model = glm::scale(model, glm::vec3((float)(TABLE_WIDTH * SquareSize), 0.0f, 0.0f));
		ResourceManager::GetShader("line").SetMatrix4f("model", model);

		glDrawArrays(GL_LINES, 0, 2);
	}

	// draw columns
	for (int x = TableUpX; x <= TableUpX + TABLE_WIDTH * SquareSize; x += SquareSize)
	{
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(x, TableUpY, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3((float)(TABLE_HEIGHT * SquareSize), 0.0f, 0.0f));
		ResourceManager::GetShader("line").SetMatrix4f("model", model);

		glDrawArrays(GL_LINES, 0, 2);
	}

	glBindVertexArray(0);
}

