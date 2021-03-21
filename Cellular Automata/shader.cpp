#include "shader.h"

#include <iostream>

Shader::Shader()
{

}

void Shader::Use()
{
	glUseProgram(ID);
}

void Shader::Compile(const char* vertexSource, const char* fragmentSource, const char* geometrySource)
{
	GLuint sVertex, sFragment, sGeometry;

	// vertex shader
	sVertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(sVertex, 1, &vertexSource, NULL);
	glCompileShader(sVertex);
	CheckCompileErrors(sVertex, "VERTEX");

	// fragment shader
	sFragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(sFragment, 1, &fragmentSource, NULL);
	glCompileShader(sFragment);
	CheckCompileErrors(sFragment, "FRAGMENT");

	// geometry shader
	if (geometrySource)
	{
		sGeometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(sGeometry, 1, &geometrySource, NULL);
		glCompileShader(sGeometry);
		CheckCompileErrors(sGeometry, "GEOMETRY");
	}

	// shader program
	ID = glCreateProgram();
	glAttachShader(ID, sVertex);
	glAttachShader(ID, sFragment);
	if (geometrySource)
		glAttachShader(ID, sGeometry);

	glLinkProgram(ID);
	CheckCompileErrors(ID, "PROGRAM");

	// delete shaders
	glDeleteShader(sVertex);
	glDeleteShader(sFragment);
	if (geometrySource)
		glDeleteShader(sGeometry);
}

void Shader::SetFloat(const char* name, float value)
{
	glUniform1f(glGetUniformLocation(ID, name), value);
}

void Shader::SetInteger(const char* name, int value)
{
	glUniform1i(glGetUniformLocation(ID, name), value);
}

void Shader::SetBool(const char* name, bool value)
{
	glUniform1i(glGetUniformLocation(ID, name), value);
}

void Shader::SetVector2f(const char* name, glm::vec2& value)
{
	glUniform2f(glGetUniformLocation(ID, name), value.x, value.y);
}

void Shader::SetVector3f(const char* name, const glm::vec3& value)
{
	glUniform3f(glGetUniformLocation(ID, name), value.x, value.y, value.z);
}

void Shader::SetVector4f(const char* name, glm::vec4& value)
{
	glUniform4f(glGetUniformLocation(ID, name), value.x, value.y, value.z, value.w);
}

void Shader::SetMatrix4f(const char* name, const glm::mat4& value)
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, false, glm::value_ptr(value));
}

void Shader::CheckCompileErrors(GLuint object, std::string type)
{
	int success;
	char infoLog[1024];

	if (type == "PROGRAM")
	{
		glGetProgramiv(object, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(object, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM:: Link-time error:\n" << infoLog << '\n';
		}
	}
	else
	{
		glGetShaderiv(object, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(object, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER::" << type << ": Compile-time error\n" << infoLog << '\n';
		}
	}
}

