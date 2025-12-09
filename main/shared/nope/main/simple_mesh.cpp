#include "simple_mesh.hpp"
#include <iostream>

GLuint create_simplemesh_vao( SimpleMeshData const& aMeshData )
{
	GLuint positionVBO = 0;
	glGenBuffers(1, &positionVBO);
	glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
	glBufferData(GL_ARRAY_BUFFER, aMeshData.positions.size() * sizeof(Vec3f), aMeshData.positions.data(), GL_STATIC_DRAW);

	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
	glVertexAttribPointer(
		0,  // location = 0 in vertex shader
		3, GL_FLOAT, GL_FALSE,  // 2 floats, not normalized to [0..1] (GL FALSE)
		0,  // stride = 0 indicates that there is no padding between inputs
		0   // data starts at offset 0 in the VBO.
	);
	glEnableVertexAttribArray(0);

	if (aMeshData.texcoords.size() != 0)
	{
		GLuint texcoordVBO = 0;
		glGenBuffers(1, &texcoordVBO);
		glBindBuffer(GL_ARRAY_BUFFER, texcoordVBO);
		glBufferData(GL_ARRAY_BUFFER, aMeshData.texcoords.size() * sizeof(Vec2f), aMeshData.texcoords.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, texcoordVBO);
		glVertexAttribPointer(
			1,
			2, GL_FLOAT, GL_FALSE,
			0,
			0
		);
		glEnableVertexAttribArray(1);
	}

	if (aMeshData.normals.size() != 0)
	{
		GLuint normalVBO = 0;
		glGenBuffers(1, &normalVBO);
		glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
		glBufferData(GL_ARRAY_BUFFER, aMeshData.normals.size() * sizeof(Vec3f), aMeshData.normals.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
		glVertexAttribPointer(
			2,
			3, GL_FLOAT, GL_FALSE,
			0,
			0
		);
		glEnableVertexAttribArray(2);
	}

	if (aMeshData.colors.size() != 0)
	{
		GLuint colorVBO = 0;
		glGenBuffers(1, &colorVBO);
		glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
		glBufferData(GL_ARRAY_BUFFER, aMeshData.colors.size() * sizeof(Vec3f), aMeshData.colors.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
		glVertexAttribPointer(
			11,
			3, GL_FLOAT, GL_FALSE,
			0,
			0
		);
		glEnableVertexAttribArray(11);
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return vao;
}

