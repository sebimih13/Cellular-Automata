#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>

class Shader
{
public:
	// constructor
	Shader();

	// state
	GLuint ID;

	// set the current shader as active
	void Use();

	// compile the shader from given source code
	void Compile(const char* vertexSource, const char* fragmentSource, const char* geometrySource = nullptr);

	// utility functions
	void SetFloat(const char* name, float value);
	void SetInteger(const char* name, int value);
	void SetBool(const char* name, bool value);
	void SetVector2f(const char* name, glm::vec2& value);
	void SetVector3f(const char* name, const glm::vec3& value);
	void SetVector4f(const char* name, glm::vec4& value);
	void SetMatrix4f(const char* name, const glm::mat4& value);

private:
	void CheckCompileErrors(GLuint object, std::string type);
};

