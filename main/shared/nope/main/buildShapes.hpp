#ifndef BUILDSHAPES_HPP
#define BUILDSHAPES_HPP


#include <vector>

#include <cstdlib>

#include "simple_mesh.hpp"

#include "../vmlib/vec3.hpp"
#include "../vmlib/mat44.hpp"
#include "./test_cube.hpp"



SimpleMeshData make_cube(
	Vec3f aColor = { 1.f, 1.f, 1.f },
	Mat44f aPreTransform = kIdentity44f
);

SimpleMeshData make_Tetrahedron(
	Vec3f aColor = { 1.f, 1.f, 1.f },
	Mat44f aPreTransform = kIdentity44f
);

SimpleMeshData make_cylinder(
	std::size_t aSubdivs = 16, 
	Vec3f aColor ={ 1.f, 1.f, 1.f },
	Mat44f aPreTransform = kIdentity44f
);


constexpr float const kTetrahedronPositions[] = {//非正三角锥：面abc为小面，abd和acd相等且为大面，bcd为底
	0.f,1.f,0.f,//a
	-1.f,-1.f,1.f, //c
	1.f,-1.f,1.f,//b
	
	
	0.f,1.f,0.f,//a
	1.f,-1.f,1.f,//b
	0.f,-1.f,-1.f, //d

	0.f,1.f,0.f,//a
	0.f,-1.f,-1.f, //d
	-1.f,-1.f,1.f, //c
	

	1.f,-1.f,1.f,//b
	-1.f,-1.f,1.f, //c
	0.f,-1.f,-1.f //d
	
};


/*
constexpr float const kCubePositions[] = {
	+1.f, +1.f, -1.f,
	-1.f, +1.f, -1.f,
	-1.f, +1.f, +1.f,
	+1.f, +1.f, -1.f,
	-1.f, +1.f, +1.f,
	+1.f, +1.f, +1.f,

	+1.f, -1.f, +1.f,
	+1.f, +1.f, +1.f,
	-1.f, +1.f, +1.f,
	+1.f, -1.f, +1.f,
	-1.f, +1.f, +1.f,
	-1.f, -1.f, +1.f,

	-1.f, -1.f, +1.f,
	-1.f, +1.f, +1.f,
	-1.f, +1.f, -1.f,
	-1.f, -1.f, +1.f,
	-1.f, +1.f, -1.f,
	-1.f, -1.f, -1.f,

	-1.f, -1.f, -1.f,
	+1.f, -1.f, -1.f,
	+1.f, -1.f, +1.f,
	-1.f, -1.f, -1.f,
	+1.f, -1.f, +1.f,
	-1.f, -1.f, +1.f,

	+1.f, -1.f, -1.f,
	+1.f, +1.f, -1.f,
	+1.f, +1.f, +1.f,
	+1.f, -1.f, -1.f,
	+1.f, +1.f, +1.f,
	+1.f, -1.f, +1.f,

	-1.f, -1.f, -1.f,
	-1.f, +1.f, -1.f,
	+1.f, +1.f, -1.f,
	-1.f, -1.f, -1.f,
	+1.f, +1.f, -1.f,
	+1.f, -1.f, -1.f,
};
*/
#endif // BUILDSHAPES_HPP