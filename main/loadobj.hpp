#ifndef LOADOBJ_HPP_2CF735BE_6624_413E_B6DC_B5BBA337F96F
#define LOADOBJ_HPP_2CF735BE_6624_413E_B6DC_B5BBA337F96F

#include "simple_mesh.hpp"

SimpleMeshData load_wavefront_obj( char const* aPath );

// Load OBJ with materials parsed from accompanying MTL
ModelMeshData load_wavefront_obj_mat( char const* aPath );

#endif // LOADOBJ_HPP_2CF735BE_6624_413E_B6DC_B5BBA337F96F
