#include "loadobj.hpp"

#include <rapidobj/rapidobj.hpp>

#include "../support/error.hpp"

SimpleMeshData load_wavefront_obj(char const* aPath)
{
    auto res = rapidobj::ParseFile(aPath);
    if (res.error)
        throw Error("Unable to load OBJ file '{}': {}", aPath, res.error.code.message().c_str());

    rapidobj::Triangulate(res);

    SimpleMeshData ret;

    for (auto const& shape : res.shapes)
	{
        for (size_t i = 0; i < shape.mesh.indices.size(); ++i)
		{
			auto const& idx = shape.mesh.indices[i];

            // Position
            ret.positions.emplace_back(Vec3f{
                res.attributes.positions[idx.position_index * 3 + 0],
                res.attributes.positions[idx.position_index * 3 + 1],
                res.attributes.positions[idx.position_index * 3 + 2]
                });

            // Normal
            if (!res.attributes.normals.empty() && idx.normal_index >= 0)
			{
                ret.normals.emplace_back(Vec3f{
                    res.attributes.normals[idx.normal_index * 3 + 0],
                    res.attributes.normals[idx.normal_index * 3 + 1],
                    res.attributes.normals[idx.normal_index * 3 + 2]
                    });
				}
            else
            {
                ret.normals.emplace_back(Vec3f{ 0,0,0 });
			}

            // Texcoord
            if (!res.attributes.texcoords.empty() && idx.texcoord_index >= 0)
{
                ret.texcoords.emplace_back(Vec2f{
                    res.attributes.texcoords[idx.texcoord_index * 2 + 0],
                    res.attributes.texcoords[idx.texcoord_index * 2 + 1]
                    });
	}
            else
	{
                ret.texcoords.emplace_back(Vec2f{ 0,0 });
            }

            // Color (ambient)
            Vec3f color{ 1.f, 1.f, 1.f };
            if (!shape.mesh.material_ids.empty())
			{
                int tri = int(i / 3);
				int matId = shape.mesh.material_ids[tri];
                if (matId >= 0 && matId < (int)res.materials.size())
				{
					auto const& mat = res.materials[matId];
					color = Vec3f{ mat.ambient[0], mat.ambient[1], mat.ambient[2] };
				}
			}
            ret.colors.emplace_back(color);
		}
	}

	return ret;
}

ModelMeshData load_wavefront_obj_mat(char const* aPath)
{
	auto res = rapidobj::ParseFile(aPath);
	if (res.error)
		throw Error("Unable to load OBJ file '{}': {}", aPath, res.error.code.message().c_str());

	rapidobj::Triangulate(res);

	ModelMeshData ret;
	// materials
	ret.materials.reserve(res.materials.size());
	for (auto const& m : res.materials)
	{
		MaterialData md;
		md.name = m.name;
		md.Ka = Vec3f{ m.ambient[0], m.ambient[1], m.ambient[2] };
		md.Kd = Vec3f{ m.diffuse[0], m.diffuse[1], m.diffuse[2] };
		md.Ks = Vec3f{ m.specular[0], m.specular[1], m.specular[2] };
		md.Ke = Vec3f{ m.emission[0], m.emission[1], m.emission[2] };
		md.Ns = m.shininess;
		md.Ni = m.ior;
		md.d = m.dissolve;
		md.illum = m.illum;
		md.map_Kd = m.diffuse_texname;
		ret.materials.emplace_back(md);
	}

	for (auto const& shape : res.shapes)
	{
		for (std::size_t i = 0; i < shape.mesh.indices.size(); ++i)
		{
			auto const& idx = shape.mesh.indices[i];
			ret.mesh.positions.emplace_back(Vec3f{
				res.attributes.positions[idx.position_index * 3 + 0],
				res.attributes.positions[idx.position_index * 3 + 1],
				res.attributes.positions[idx.position_index * 3 + 2]
				});

			// normals - use provided normal or default
			if (!res.attributes.normals.empty() && idx.normal_index >= 0)
				ret.mesh.normals.emplace_back(Vec3f{
					res.attributes.normals[idx.normal_index * 3 + 0],
					res.attributes.normals[idx.normal_index * 3 + 1],
					res.attributes.normals[idx.normal_index * 3 + 2]
					});
			else ret.mesh.normals.emplace_back(Vec3f{ 0.f,1.f,0.f });

			// texcoords
			if (!res.attributes.texcoords.empty() && idx.texcoord_index >= 0)
				ret.mesh.texcoords.emplace_back(Vec2f{
					res.attributes.texcoords[idx.texcoord_index * 2 + 0],
					res.attributes.texcoords[idx.texcoord_index * 2 + 1]
					});
			else ret.mesh.texcoords.emplace_back(Vec2f{ 0.f,0.f });

			// per-vertex color from ambient
			Vec3f color{ 1.f,1.f,1.f };
			int triId = -1;
			if (!shape.mesh.material_ids.empty())
			{
				int tri = int(i / 3);
				int matId = shape.mesh.material_ids[tri];
				triId = matId;
				if (matId >= 0 && matId < int(res.materials.size()))
				{
					auto const& mat = res.materials[matId];
					color = Vec3f{ mat.ambient[0], mat.ambient[1], mat.ambient[2] };
				}
			}
			ret.mesh.colors.emplace_back(color);
			if (i % 3 == 2) ret.triangleMaterialIds.emplace_back(triId);
		}
	}
	return ret;
}