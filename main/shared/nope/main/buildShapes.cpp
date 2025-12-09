#include "buildShapes.hpp"

#include "../vmlib/mat33.hpp"
#include <iostream>




SimpleMeshData make_cube(Vec3f aColor, Mat44f aPreTransform) {
	std::vector<Vec3f> pos;
	std::vector<Vec3f> nor;
    std::vector<Vec3f> col;
    
   
	for (int i = 0; i < sizeof(kCubePositions) / sizeof(kCubePositions[0]); i += 3) {
        float x = kCubePositions[i];
        float y = kCubePositions[i + 1];
        float z = kCubePositions[i + 2];

        Vec3f tempVec{ x,y,z };
		pos.emplace_back(tempVec);
		nor.emplace_back(tempVec);
        col.emplace_back(aColor);
	}
    
    // position transform
    std::vector<Vec3f> finalPos;
    for (auto& p : pos)
    {
        Vec4f p4{ p.x, p.y, p.z, 1.f };
        Vec4f t = aPreTransform * p4;
        t /= t.w;
        p = Vec3f{ t.x, t.y, t.z };
        finalPos.emplace_back(p);
    }
    pos = finalPos;

    // normal transform
    std::vector<Vec3f> finalNor;
    for (auto& n : nor)
    {
        Vec4f n4{ n.x, n.y, n.z, 1.f };
        Vec4f t = aPreTransform * n4;
        t /= t.w;
        n = Vec3f{ t.x, t.y, t.z };
        finalNor.emplace_back(n);
    }
    nor = finalNor;


    std::vector<Vec2f> placeholder;


    return SimpleMeshData{ std::move(pos), std::move(placeholder), std::move(nor), std::move(col) };
}



SimpleMeshData make_Tetrahedron(Vec3f aColor, Mat44f aPreTransform) {
    std::vector<Vec3f> pos;
    std::vector<Vec3f> nor;
    std::vector<Vec3f> col;


    for (int i = 0; i < sizeof(kTetrahedronPositions) / sizeof(kTetrahedronPositions[0]); i += 3) {
        float x = kTetrahedronPositions[i];
        float y = kTetrahedronPositions[i + 1];
        float z = kTetrahedronPositions[i + 2];

        Vec3f tempVec{ x,y,z };
        pos.emplace_back(tempVec);
        nor.emplace_back(tempVec);
        col.emplace_back(aColor);
    }

    // position transform
    std::vector<Vec3f> finalPos;
    for (auto& p : pos)
    {
        Vec4f p4{ p.x, p.y, p.z, 1.f };
        Vec4f t = aPreTransform * p4;
        t /= t.w;
        p = Vec3f{ t.x, t.y, t.z };
        finalPos.emplace_back(p);
    }
    pos = finalPos;

    // normal transform
    std::vector<Vec3f> finalNor;
    for (auto& n : nor)
    {
        Vec4f n4{ n.x, n.y, n.z, 1.f };
        Vec4f t = aPreTransform * n4;
        t /= t.w;
        n = Vec3f{ t.x, t.y, t.z };
        finalNor.emplace_back(n);
    }
    nor = finalNor;


    std::vector<Vec2f> placeholder;


    return SimpleMeshData{ std::move(pos), std::move(placeholder), std::move(nor), std::move(col) };
}


SimpleMeshData make_cylinder(std::size_t aSubdivs, Vec3f aColor, Mat44f aPreTransform)
{
    std::vector<Vec3f> pos;
    std::vector<Vec3f> nor;

    float prevY = std::cos(0.f);
    float prevZ = std::sin(0.f);

    // create shell
    for (std::size_t i = 0; i < aSubdivs; ++i) {
        float const angle = (i + 1) / float(aSubdivs) * 2.f * 3.1415926f;
        float y = std::cos(angle);
        float z = std::sin(angle);

        pos.emplace_back(Vec3f{ 0.f, prevY, prevZ });
        pos.emplace_back(Vec3f{ 0.f, y, z });
        pos.emplace_back(Vec3f{ 1.f, prevY, prevZ });

        pos.emplace_back(Vec3f{ 0.f, y, z });
        pos.emplace_back(Vec3f{ 1.f, y, z });
        pos.emplace_back(Vec3f{ 1.f, prevY, prevZ });

        // normals
        for (int j = 0; j < 6; ++j) {
            nor.emplace_back(Vec3f{ 0.f, y, z });
        }

        prevY = y;
        prevZ = z;
    }
    // create cap
    
    if (1)
    {
        Vec3f topCenter = Vec3f{ 1.f, 0.f, 0.f };
        prevY = std::cos(0.f);
        prevZ = std::sin(0.f);

        for (std::size_t i = 0; i < aSubdivs; ++i) {
            float const angle = (i + 1) / float(aSubdivs) * 2.f * 3.1415926f;
            float y = std::cos(angle);
            float z = std::sin(angle);

            pos.emplace_back(topCenter);
            pos.emplace_back(Vec3f{ 1.f, prevY, prevZ });
            pos.emplace_back(Vec3f{ 1.f, y, z });

            // normals
            for (int j = 0; j < 3; ++j) {
                nor.emplace_back(Vec3f{ 1.f, 0.f, 0.f });
            }

            prevY = y;
            prevZ = z;
        }

        Vec3f bottomCenter = Vec3f{ 0.f, 0.f, 0.f };
        prevY = std::cos(0.f);
        prevZ = std::sin(0.f);

        for (std::size_t i = 0; i < aSubdivs; ++i) {
            float const angle = (i + 1) / float(aSubdivs) * 2.f * 3.1415926f;
            float y = std::cos(angle);
            float z = std::sin(angle);

            pos.emplace_back(bottomCenter);
            pos.emplace_back(Vec3f{ 0.f, y, z });
            pos.emplace_back(Vec3f{ 0.f, prevY, prevZ });

            // normals
            for (int j = 0; j < 3; ++j) {
                nor.emplace_back(Vec3f{ -1.f, 0.f, 0.f });
            }

            prevY = y;
            prevZ = z;
        }
    }

    // position transform
    std::vector<Vec3f> finalPos;
    for (auto& p : pos)
    {
        Vec4f p4{ p.x, p.y, p.z, 1.f };
        Vec4f t = aPreTransform * p4;
        t /= t.w;
        p = Vec3f{ t.x, t.y, t.z };
        finalPos.emplace_back(p);
    }
    pos = finalPos;

    // normal transform
    std::vector<Vec3f> finalNor;
    for (auto& n : nor)
    {
        Vec4f n4{ n.x, n.y, n.z, 1.f };
        Vec4f t = aPreTransform * n4;
        t /= t.w;
        n = Vec3f{ t.x, t.y, t.z };
        finalNor.emplace_back(n);
        // std::cout << n.x << "|" << n.y << "|" << n.z << std::endl;
    }
    nor = finalNor;

    std::vector col(pos.size(), aColor);
    std::vector<Vec2f> placeholder;

    return SimpleMeshData{ std::move(pos), std::move(placeholder), std::move(nor), std::move(col) };
}
