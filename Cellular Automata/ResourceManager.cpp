#include "ResourceManager.h"

#include <iostream>
#include <fstream>
#include <sstream>

// Instantiate static variables
std::map<std::string, Shader> ResourceManager::Shaders;

ResourceManager::ResourceManager()
{

}

void ResourceManager::LoadShader(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile, std::string name)
{
	Shaders[name] = LoadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
}

Shader& ResourceManager::GetShader(std::string name)
{
	return Shaders[name];
}

void ResourceManager::Clear()
{
	for (auto iter : Shaders)
		glDeleteProgram(iter.second.ID);
}

Shader ResourceManager::LoadShaderFromFile(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile)
{
	// 1. retrieve the vertex/fragment source code from file path
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	
	try
	{
		// open files
		std::ifstream vertexShaderFile(vShaderFile);
		std::ifstream fragmentShaderFile(fShaderFile);

		std::stringstream vShaderStream, fShaderStream;

		// read file's buffers contents into streams
		vShaderStream << vertexShaderFile.rdbuf();
		fShaderStream << fragmentShaderFile.rdbuf();

		// close file handlers
		vertexShaderFile.close();
		fragmentShaderFile.close();

		// convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();

		if (gShaderFile)
		{
			std::ifstream geometryShaderFile(gShaderFile);
			std::stringstream gShaderStream;
			gShaderStream << geometryShaderFile.rdbuf();
			geometryShaderFile.close();
			geometryCode = gShaderStream.str();
		}
	}
	catch (std::exception e)
	{
		std::cout << "ERROR::SHADER:: Failed to read shader from file" << std::endl;
	}

	// 2. create shader object from source code
	Shader shader;
	shader.Compile(vertexCode.c_str(), fragmentCode.c_str(), gShaderFile ? geometryCode.c_str() : nullptr);
	return shader;
}
