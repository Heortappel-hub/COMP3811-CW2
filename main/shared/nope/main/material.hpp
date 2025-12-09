#include <glad.h>

#include <vector>

#include "../vmlib/vec3.hpp"
#include "../vmlib/vec2.hpp"

struct Material {
    int materialID;
    float Ns; // Ns exponent
    Vec3f Ka; // ambient
    Vec3f Kd; // diffuse
    Vec3f Ks; // specular
    Vec3f Ke; // emissive
    float Ni; // optical density
    float d; // d factor
    int illum; // illumination
};