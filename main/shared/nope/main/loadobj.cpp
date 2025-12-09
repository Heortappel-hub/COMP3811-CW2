#include "loadobj.hpp"

#include <rapidobj/rapidobj.hpp>

#include "../support/error.hpp"

#include <iostream>

SimpleMeshData load_wavefront_obj_simplemesh(char const* aPath)
{
	auto result = rapidobj::ParseFile( aPath );
	if( result.error )
		throw Error( "Unable to load OBJ file ¡¯%s¡¯: %s", aPath, result.error.code.message().c_str() );
	// OBJ files can define faces that are not triangles. However, OpenGL will only render triangles (and lines9g// and points), so we must triangulate any faces that are not already triangles. Fortunately, rapidobj can do this for us
	rapidobj::Triangulate( result );
	// Convert the OBJ data into a SimpleMeshData structure. For now, we simply turn the object into a triangle soup, ignoring the indexing information that the OBJ file contains.
	SimpleMeshData ret;
	for( auto const& shape : result.shapes )
	{
		for( std::size_t i = 0; i < shape.mesh.indices.size(); ++i )
		{
			auto const& idx = shape.mesh.indices[i];
			ret.positions.emplace_back(Vec3f{ result.attributes.positions[idx.position_index * 3 + 0],result.attributes.positions[idx.position_index * 3 + 1],result.attributes.positions[idx.position_index * 3 + 2] });
			// Always triangles, so we can find the face index by dividing the vertex index by three30
			
			//auto const& mat = result.materials[shape.mesh.material_ids[i/3]];
			// Just replicate the material ambient color for each vertex...
			// ret.colors.emplace_back( Vec3f{mat.ambient[0],mat.ambient[1],mat.ambient[2]} );
			
			// Read texture information
			ret.texcoords.emplace_back(Vec2f{ result.attributes.texcoords[idx.texcoord_index * 2 + 0],result.attributes.texcoords[idx.texcoord_index * 2 + 1] });

			// Read normal information
			ret.normals.emplace_back(Vec3f{ result.attributes.normals[idx.normal_index * 3 + 0],result.attributes.normals[idx.normal_index * 3 + 1],result.attributes.normals[idx.normal_index * 3 + 2] });
		}
	}
	return ret;
}

ModelMeshData load_wavefront_obj_modelmesh(char const* aPath)
{
	auto result = rapidobj::ParseFile(aPath);
	if (result.error)
		throw Error("Unable to load OBJ file ¡¯%s¡¯: %s", aPath, result.error.code.message().c_str());
	// OBJ files can define faces that are not triangles. However, OpenGL will only render triangles (and lines9g// and points), so we must triangulate any faces that are not already triangles. Fortunately, rapidobj can do this for us
	rapidobj::Triangulate(result);
	// Convert the OBJ data into a SimpleMeshData structure. For now, we simply turn the object into a triangle soup, ignoring the indexing information that the OBJ file contains.
	ModelMeshData ret;
	for (auto const& shape : result.shapes)
	{
		for (std::size_t i = 0; i < shape.mesh.indices.size(); ++i)
		{
			auto const& idx = shape.mesh.indices[i];
			// Read position information
			ret.positions.emplace_back(Vec3f{ result.attributes.positions[idx.position_index * 3 + 0],result.attributes.positions[idx.position_index * 3 + 1],result.attributes.positions[idx.position_index * 3 + 2] });
			
			// Read texture information
			ret.texcoords.emplace_back(Vec2f{ result.attributes.texcoords[idx.texcoord_index * 2 + 0],result.attributes.texcoords[idx.texcoord_index * 2 + 1] });

			// Read normal information
			ret.normals.emplace_back(Vec3f{ result.attributes.normals[idx.normal_index * 3 + 0],result.attributes.normals[idx.normal_index * 3 + 1],result.attributes.normals[idx.normal_index * 3 + 2] });

			// Read shininess information
			ret.Ns.emplace_back(result.materials[shape.mesh.material_ids[0]].shininess);

			// Read shininess information
			ret.Ka.emplace_back(Vec3f{ result.materials[shape.mesh.material_ids[0]].ambient[0],result.materials[shape.mesh.material_ids[0]].ambient[1],result.materials[shape.mesh.material_ids[0]].ambient[2] });

			// Read shininess information
			ret.Kd.emplace_back(Vec3f{ result.materials[shape.mesh.material_ids[0]].diffuse[0],result.materials[shape.mesh.material_ids[0]].diffuse[1],result.materials[shape.mesh.material_ids[0]].diffuse[2] });

			// Read shininess information
			ret.Ks.emplace_back(Vec3f{ result.materials[shape.mesh.material_ids[0]].specular[0],result.materials[shape.mesh.material_ids[0]].specular[1],result.materials[shape.mesh.material_ids[0]].specular[2] });

			// Read shininess information
			ret.Ke.emplace_back(Vec3f{ result.materials[shape.mesh.material_ids[0]].emission[0],result.materials[shape.mesh.material_ids[0]].emission[1],result.materials[shape.mesh.material_ids[0]].emission[2] });

			// Read shininess information
			ret.Ni.emplace_back(result.materials[shape.mesh.material_ids[0]].ior);

			// Read shininess information
			ret.d.emplace_back(result.materials[shape.mesh.material_ids[0]].dissolve);

			// Read shininess information
			ret.illum.emplace_back(result.materials[shape.mesh.material_ids[0]].illum);
		}
	}
	return ret;
}