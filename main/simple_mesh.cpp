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


GLuint create_vao_mat( ModelMeshData const& aMeshData )
{
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
	// color ->5 (match your shader)
	if( !aMeshData.mesh.colors.empty() )
	{
		GLuint vbo =0; glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER,
			aMeshData.mesh.colors.size() * sizeof(Vec3f),
			aMeshData.mesh.colors.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(5,3, GL_FLOAT, GL_FALSE,0, (void*)0);
		glEnableVertexAttribArray(5);
	}

	glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindVertexArray(0);
	return vao;
}