#include <glad.h>

#include <vector>

#include "../vmlib/vec3.hpp"
#include "../vmlib/vec2.hpp"

struct ModelMeshData
{
    std::vector<Vec3f> positions;
    std::vector<Vec2f> texcoords;
    std::vector<Vec3f> normals;
    std::vector<float> Ns; // Ns exponent
    std::vector<Vec3f> Ka; // ambient
    std::vector<Vec3f> Kd; // diffuse
    std::vector<Vec3f> Ks; // specular
    std::vector<Vec3f> Ke; // emissive
    std::vector<float> Ni; // optical density
    std::vector<float> d; // d factor
    std::vector<float> illum; // illumination
};

GLuint create_modelmesh_vao(ModelMeshData const&);