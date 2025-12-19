#include "simple_mesh.hpp"

SimpleMeshData concatenate( SimpleMeshData aM, SimpleMeshData const& aN )
{
	aM.positions.insert( aM.positions.end(), aN.positions.begin(), aN.positions.end() );
	aM.colors.insert( aM.colors.end(), aN.colors.begin(), aN.colors.end() );
	return aM;
}


GLuint create_vao(SimpleMeshData const& aMeshData)
{
    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Position VBO ¡ú location 0

    if (!aMeshData.positions.empty())
    {
        GLuint positionVBO = 0;
        glGenBuffers(1, &positionVBO);
        glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
        glBufferData(GL_ARRAY_BUFFER,
            aMeshData.positions.size() * sizeof(Vec3f),
            aMeshData.positions.data(),
            GL_STATIC_DRAW);

        glVertexAttribPointer(
            0,            // layout(location = 0)
            3, GL_FLOAT,
            GL_FALSE,
            0, (void*)0
        );
        glEnableVertexAttribArray(0);
    }


    // Texcoord VBO ¡ú location 1

    if (!aMeshData.texcoords.empty())
    {
        GLuint texcoordVBO = 0;
        glGenBuffers(1, &texcoordVBO);
        glBindBuffer(GL_ARRAY_BUFFER, texcoordVBO);
        glBufferData(GL_ARRAY_BUFFER,
            aMeshData.texcoords.size() * sizeof(Vec2f),
            aMeshData.texcoords.data(),
            GL_STATIC_DRAW);

        glVertexAttribPointer(
            1,            // layout(location = 1)
            2, GL_FLOAT,
            GL_FALSE,
            0, (void*)0
        );
        glEnableVertexAttribArray(1);
    }


    // Normal VBO ¡ú location 2

    if (!aMeshData.normals.empty())
    {
        GLuint normalVBO = 0;
        glGenBuffers(1, &normalVBO);
        glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
        glBufferData(GL_ARRAY_BUFFER,
            aMeshData.normals.size() * sizeof(Vec3f),
            aMeshData.normals.data(),
            GL_STATIC_DRAW);

        glVertexAttribPointer(
            2,            // layout(location = 2)
            3, GL_FLOAT,
            GL_FALSE,
            0, (void*)0
        );
        glEnableVertexAttribArray(2);
    }


    // Color VBO ¡ú location 5
    if (!aMeshData.colors.empty())
    {
        GLuint colorVBO = 0;
        glGenBuffers(1, &colorVBO);
        glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
        glBufferData(GL_ARRAY_BUFFER,
            aMeshData.colors.size() * sizeof(Vec3f),
            aMeshData.colors.data(),
            GL_STATIC_DRAW);

        glVertexAttribPointer(
            5,           // layout(location = 5)
            3, GL_FLOAT,
            GL_FALSE,
            0, (void*)0
        );
        glEnableVertexAttribArray(5);
    }


    // Cleanup
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return vao;
}


GLuint create_vao_mat( ModelMeshData const& aMeshData ){
	GLuint vao =0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// position ->0
	if( !aMeshData.mesh.positions.empty() )
	{
		GLuint vbo =0; glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER,
			aMeshData.mesh.positions.size() * sizeof(Vec3f),
			aMeshData.mesh.positions.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0,3, GL_FLOAT, GL_FALSE,0, (void*)0);
		glEnableVertexAttribArray(0);
	}
	// texcoord ->1
	if( !aMeshData.mesh.texcoords.empty() )
	{
		GLuint vbo =0; glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER,
			aMeshData.mesh.texcoords.size() * sizeof(Vec2f),
			aMeshData.mesh.texcoords.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(1,2, GL_FLOAT, GL_FALSE,0, (void*)0);
		glEnableVertexAttribArray(1);
	}
	// normal ->2
	if( !aMeshData.mesh.normals.empty() )
	{
		GLuint vbo =0; glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER,
			aMeshData.mesh.normals.size() * sizeof(Vec3f),
			aMeshData.mesh.normals.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(2,3, GL_FLOAT, GL_FALSE,0, (void*)0);
		glEnableVertexAttribArray(2);
	}

	// Build per-vertex material attributes from triangleMaterialIds
	size_t vertexCount = aMeshData.mesh.positions.size();
	std::vector<Vec3f> perVertexKa(vertexCount, Vec3f{0.1f,0.1f,0.1f});
	std::vector<Vec3f> perVertexKd(vertexCount, Vec3f{0.8f,0.8f,0.8f});
	std::vector<Vec3f> perVertexKs(vertexCount, Vec3f{0.2f,0.2f,0.2f});
	std::vector<Vec3f> perVertexKe(vertexCount, Vec3f{0.f,0.f,0.f});
	std::vector<float> perVertexNs(vertexCount, 16.f);
	std::vector<float> perVertexNi(vertexCount, 1.45f);
	std::vector<float> perVertexD(vertexCount, 1.0f);
	std::vector<int> perVertexIllum(vertexCount, 2);

	if (!aMeshData.materials.empty() && !aMeshData.triangleMaterialIds.empty())
	{
		for (size_t tri = 0; tri < aMeshData.triangleMaterialIds.size(); ++tri)
		{
			int matId = aMeshData.triangleMaterialIds[tri];
			if (matId >= 0 && matId < (int)aMeshData.materials.size())
			{
				auto const& mat = aMeshData.materials[matId];
				for (int v = 0; v < 3; ++v)
				{
					size_t vertIdx = tri * 3 + v;
					if (vertIdx < vertexCount)
					{
						perVertexKa[vertIdx] = mat.Ka;
						perVertexKd[vertIdx] = mat.Kd;
						perVertexKs[vertIdx] = mat.Ks;
						perVertexKe[vertIdx] = mat.Ke;
						perVertexNs[vertIdx] = mat.Ns;
						perVertexNi[vertIdx] = mat.Ni;
						perVertexD[vertIdx] = mat.d;
						perVertexIllum[vertIdx] = mat.illum;
					}
				}
			}
		}
	}

	// Ns -> location 3
	{
		GLuint vbo = 0; glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, perVertexNs.size() * sizeof(float), perVertexNs.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(3);
	}
	// Ka -> location 4
	{
		GLuint vbo = 0; glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, perVertexKa.size() * sizeof(Vec3f), perVertexKa.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(4);
	}
	// Kd -> location 5 
	{
		GLuint vbo = 0; glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, perVertexKd.size() * sizeof(Vec3f), perVertexKd.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(5);
	}
	// Ks -> location 6 
	{
		GLuint vbo = 0; glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, perVertexKs.size() * sizeof(Vec3f), perVertexKs.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(6);
	}
	// Ke -> location 7 
	{
		GLuint vbo = 0; glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, perVertexKe.size() * sizeof(Vec3f), perVertexKe.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(7);
	}
	// Ni -> location 8
	{
		GLuint vbo = 0; glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, perVertexNi.size() * sizeof(float), perVertexNi.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(8, 1, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(8);
	}
	// d -> location 9
	{
		GLuint vbo = 0; glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, perVertexD.size() * sizeof(float), perVertexD.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(9, 1, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(9);
	}
	// illum -> location 10
	{
		GLuint vbo = 0; glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, perVertexIllum.size() * sizeof(int), perVertexIllum.data(), GL_STATIC_DRAW);
		glVertexAttribIPointer(10, 1, GL_INT, 0, (void*)0);
		glEnableVertexAttribArray(10);
	}

	glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindVertexArray(0);
	return vao;
}

