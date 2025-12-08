#ifndef SIMPLE_MESH_HPP_C6B749D6_C83B_434C_9E58_F05FC27FEFC9
#define SIMPLE_MESH_HPP_C6B749D6_C83B_434C_9E58_F05FC27FEFC9

#include <glad/glad.h>

#include <vector>
#include <string>

#include "../vmlib/vec3.hpp"
#include "../vmlib/vec2.hpp"

struct SimpleMeshData
{
	std::vector<Vec3f> positions;
	std::vector<Vec3f> colors;
	std::vector<Vec2f> texcoords;
	std::vector<Vec3f> normals;
};

// Material description parsed from .mtl
struct MaterialData
{
	std::string name;
	Vec3f Ka{0.f,0.f,0.f}; // ambient
	Vec3f Kd{1.f,1.f,1.f}; // diffuse
	Vec3f Ks{0.f,0.f,0.f}; // specular
	Vec3f Ke{0.f,0.f,0.f}; // emission
	float Ns{0.f}; // shininess
	float Ni{1.f}; // index of refraction
	float d{1.f}; // dissolve (opacity)
	int illum{2}; // illumination model
	std::string map_Kd; // diffuse texture path (optional)
};

// Mesh plus materials and per-triangle material ids
struct ModelMeshData
{
	SimpleMeshData mesh;
	std::vector<MaterialData> materials;
	std::vector<int> triangleMaterialIds; // size == vertex_count/3; -1 if none
};

SimpleMeshData concatenate( SimpleMeshData, SimpleMeshData const& );

GLuint create_vao( SimpleMeshData const& );

GLuint create_vao_mat( ModelMeshData const& );

#endif // SIMPLE_MESH_HPP_C6B749D6_C83B_434C_9E58_F05FC27FEFC9
