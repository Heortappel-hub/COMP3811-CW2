#ifndef LOADOBJ_HPP_2CF735BE_6624_413E_B6DC_B5BBA337F96F
#define LOADOBJ_HPP_2CF735BE_6624_413E_B6DC_B5BBA337F96F

#include "simple_mesh.hpp"

#include "model_mesh.hpp"

SimpleMeshData load_wavefront_obj_simplemesh( char const* aPath );

ModelMeshData load_wavefront_obj_modelmesh(char const* aPath);

#endif // LOADOBJ_HPP_2CF735BE_6624_413E_B6DC_B5BBA337F96F
