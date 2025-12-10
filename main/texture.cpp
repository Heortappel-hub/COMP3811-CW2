#include "texture.hpp"

#include <cstdio>

#include "../support/error.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "../third_party/stb/include/stb_image.h"

GLuint load_texture_2d(char const* aPath)
{
	stbi_set_flip_vertically_on_load(true); 
	
	int width, height, channelsInFile;
	stbi_uc* data = stbi_load(aPath, &width, &height, &channelsInFile, 4);
	
	if (!data)
	{
		throw Error("Unable to load texture image '%s': %s", aPath, stbi_failure_reason());
	}

	// Create OpenGL texture object
	GLuint texId = 0;
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);
	
	// Upload texture data
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_SRGB8_ALPHA8,  // Use sRGB for proper color space
		width,
		height,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		data
	);

	// Free CPU memory immediately after upload
	stbi_image_free(data);

	// Generate mipmaps (after upload, before setting parameters)
	glGenerateMipmap(GL_TEXTURE_2D);

	// Set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	
	// Set wrap mode (CLAMP_TO_EDGE for terrain orthophoto)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);

	return texId;
}
