#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>

class Texture2D
{
public:
	// constructor
	Texture2D();

	// holds the ID of the texture object
	GLuint ID;

	// texture image dimensions : width and height of loaded image in pixels
	unsigned int Width, Height;

	// texture format
	unsigned int Internal_Format;	// format of texture object
	unsigned int Image_Format;		// format of loaded image

	// texture configuration
	unsigned int Wrap_S;			// wrapping mode for S axis
	unsigned int Wrap_T;			// wrapping mode for T axis
	unsigned int Filter_Min;		// filtering mode if texture pixels < screen pixels
	unsigned int Filter_Max;		// filtering mode if texture pixels > screen pixels

	// generate texture from image data
	void Generate(unsigned int width, unsigned int height, unsigned char* data);

	// bind the texture as the current active GL_TEXTURE_2D texture object
	void Bind();
};

#endif
