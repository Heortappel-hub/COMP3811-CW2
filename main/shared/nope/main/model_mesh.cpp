#include "model_mesh.hpp"
#include <iostream>

GLuint create_modelmesh_vao(ModelMeshData const& aMeshData)
{
	/*
	std::cout << aMeshData.positions.size() << std::endl;
	std::cout << aMeshData.texcoords.size() << std::endl;
	std::cout << aMeshData.normals.size() << std::endl;
	std::cout << aMeshData.Ns.size() << std::endl;
	std::cout << aMeshData.Ka.size() << std::endl;
	std::cout << aMeshData.Kd.size() << std::endl;
	std::cout << aMeshData.Ks.size() << std::endl;
	std::cout << aMeshData.Ke.size() << std::endl;
	std::cout << aMeshData.Ni.size() << std::endl;
	std::cout << aMeshData.d.size() << std::endl;
	std::cout << aMeshData.illum.size() << std::endl;
	*/

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

	if (aMeshData.Ns.size() != 0)
	{
		GLuint NsVBO = 0;
		glGenBuffers(1, &NsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, NsVBO);
		glBufferData(GL_ARRAY_BUFFER, aMeshData.Ns.size() * sizeof(float), aMeshData.Ns.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, NsVBO);
		glVertexAttribPointer(
			3,
			1, GL_FLOAT, GL_FALSE,
			0,
			0
		);
		glEnableVertexAttribArray(3);
	}

	if (aMeshData.Ka.size() != 0)
	{
		GLuint KaVBO = 0;
		glGenBuffers(1, &KaVBO);
		glBindBuffer(GL_ARRAY_BUFFER, KaVBO);
		glBufferData(GL_ARRAY_BUFFER, aMeshData.Ka.size() * sizeof(Vec3f), aMeshData.Ka.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, KaVBO);
		glVertexAttribPointer(
			4,
			3, GL_FLOAT, GL_FALSE,
			0,
			0
		);
		glEnableVertexAttribArray(4);
	}

	if (aMeshData.Kd.size() != 0)
	{
		GLuint KdVBO = 0;
		glGenBuffers(1, &KdVBO);
		glBindBuffer(GL_ARRAY_BUFFER, KdVBO);
		glBufferData(GL_ARRAY_BUFFER, aMeshData.Kd.size() * sizeof(Vec3f), aMeshData.Kd.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, KdVBO);
		glVertexAttribPointer(
			5,
			3, GL_FLOAT, GL_FALSE,
			0,
			0
		);
		glEnableVertexAttribArray(5);
	}

	if (aMeshData.Ks.size() != 0)
	{
		GLuint KsVBO = 0;
		glGenBuffers(1, &KsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, KsVBO);
		glBufferData(GL_ARRAY_BUFFER, aMeshData.Ks.size() * sizeof(Vec3f), aMeshData.Ks.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, KsVBO);
		glVertexAttribPointer(
			6,
			3, GL_FLOAT, GL_FALSE,
			0,
			0
		);
		glEnableVertexAttribArray(6);
	}

	if (aMeshData.Ke.size() != 0)
	{
		GLuint KeVBO = 0;
		glGenBuffers(1, &KeVBO);
		glBindBuffer(GL_ARRAY_BUFFER, KeVBO);
		glBufferData(GL_ARRAY_BUFFER, aMeshData.Ke.size() * sizeof(Vec3f), aMeshData.Ke.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, KeVBO);
		glVertexAttribPointer(
			7,
			3, GL_FLOAT, GL_FALSE,
			0,
			0
		);
		glEnableVertexAttribArray(7);
	}

	if (aMeshData.Ni.size() != 0)
	{
		GLuint NiVBO = 0;
		glGenBuffers(1, &NiVBO);
		glBindBuffer(GL_ARRAY_BUFFER, NiVBO);
		glBufferData(GL_ARRAY_BUFFER, aMeshData.Ni.size() * sizeof(float), aMeshData.Ni.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, NiVBO);
		glVertexAttribPointer(
			8,
			1, GL_FLOAT, GL_FALSE,
			0,
			0
		);
		glEnableVertexAttribArray(8);
	}

	if (aMeshData.d.size() != 0)
	{
		GLuint dVBO = 0;
		glGenBuffers(1, &dVBO);
		glBindBuffer(GL_ARRAY_BUFFER, dVBO);
		glBufferData(GL_ARRAY_BUFFER, aMeshData.d.size() * sizeof(float), aMeshData.d.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, dVBO);
		glVertexAttribPointer(
			9,
			1, GL_FLOAT, GL_FALSE,
			0,
			0
		);
		glEnableVertexAttribArray(9);
	}

	if (aMeshData.illum.size() != 0)
	{
		GLuint illumVBO = 0;
		glGenBuffers(1, &illumVBO);
		glBindBuffer(GL_ARRAY_BUFFER, illumVBO);
		glBufferData(GL_ARRAY_BUFFER, aMeshData.illum.size() * sizeof(float), aMeshData.illum.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, illumVBO);
		glVertexAttribPointer(
			10,
			1, GL_FLOAT, GL_FALSE,
			0,
			0
		);
		glEnableVertexAttribArray(10);
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return vao;
}