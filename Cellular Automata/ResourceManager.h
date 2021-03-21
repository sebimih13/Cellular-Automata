#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <glad/glad.h>

#include <map>
#include <string>

#include "shader.h"

class ResourceManager
{
public:
	// constructor
	ResourceManager();

	// load and generate a shader program from files: vertex, fragment, geometry
	static void LoadShader(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile, std::string name);

	// retrieve a stored shader
	static Shader& GetShader(std::string name);

	// de-allocate all loaded resources
	static void Clear();

private:
	// resource storage
	static std::map<std::string, Shader> Shaders;

	// load and generate a shader from file
	static Shader LoadShaderFromFile(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile = nullptr);
};

#endif
