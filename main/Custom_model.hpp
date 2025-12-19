#include <glad/glad.h>
#include <numbers>
#include <cmath>

#include "simple_mesh.hpp"


Vec3f rgb_to_float(int r, int g, int b) {
	return Vec3f{
		r / 255.f,
		g / 255.f,
		b / 255.f
	};
}

auto tardis_blue = rgb_to_float(25, 25, 112);
auto tardis_white = rgb_to_float(245, 245, 245);


// 创建可自定义尺寸的长方体
SimpleMeshData make_cuboid(float width, float height, float depth, Vec3f color = {1.f, 1.f, 1.f})
{
	SimpleMeshData mesh;
	
	float hx = width * 0.5f;
	float hy = height * 0.5f;
	float hz = depth * 0.5f;

	// 36 个顶点
	mesh.positions = {
		// Front face 
		{ -hx, -hy,  hz }, {  hx, -hy,  hz }, {  hx,  hy,  hz },
		{ -hx, -hy,  hz }, {  hx,  hy,  hz }, { -hx,  hy,  hz },
		// Back face 
		{ -hx, -hy, -hz }, { -hx,  hy, -hz }, {  hx,  hy, -hz },
		{ -hx, -hy, -hz }, {  hx,  hy, -hz }, {  hx, -hy, -hz },
		// Left face
		{ -hx, -hy, -hz }, { -hx, -hy,  hz }, { -hx,  hy,  hz },
		{ -hx, -hy, -hz }, { -hx,  hy,  hz }, { -hx,  hy, -hz },
		// Right face
		{  hx, -hy, -hz }, {  hx,  hy, -hz }, {  hx,  hy,  hz },
		{  hx, -hy, -hz }, {hx,  hy,  hz }, {  hx, -hy,  hz },
		// Top face
		{ -hx,  hy, -hz }, { -hx,  hy,  hz }, {  hx,  hy,  hz },
		{ -hx,  hy, -hz }, {  hx,  hy,  hz }, {  hx,  hy, -hz },
		// Bottom face 
		{ -hx, -hy, -hz }, {  hx, -hy, -hz }, {  hx, -hy,  hz },
		{ -hx, -hy, -hz }, {  hx, -hy,  hz }, { -hx, -hy,  hz }
	};

	// Normals
	mesh.normals = {
		// Front (Z+)
		{ 0.f, 0.f, 1.f }, { 0.f, 0.f, 1.f }, { 0.f, 0.f, 1.f },
		{ 0.f, 0.f, 1.f }, { 0.f, 0.f, 1.f }, { 0.f, 0.f, 1.f },
		// Back (Z-)
		{ 0.f, 0.f, -1.f }, { 0.f, 0.f, -1.f }, { 0.f, 0.f, -1.f },
		{ 0.f, 0.f, -1.f }, { 0.f, 0.f, -1.f }, { 0.f, 0.f, -1.f },
		// Left (X-)
		{ -1.f, 0.f, 0.f }, { -1.f, 0.f, 0.f }, { -1.f, 0.f, 0.f },
		{ -1.f, 0.f, 0.f }, { -1.f, 0.f, 0.f }, { -1.f, 0.f, 0.f },
		// Right (X+)
		{ 1.f, 0.f, 0.f }, { 1.f, 0.f, 0.f }, { 1.f, 0.f, 0.f },
		{ 1.f, 0.f, 0.f }, { 1.f, 0.f, 0.f }, { 1.f, 0.f, 0.f },
		// Top (Y+)
		{ 0.f, 1.f, 0.f }, { 0.f, 1.f, 0.f }, { 0.f, 1.f, 0.f },
		{ 0.f, 1.f, 0.f }, { 0.f, 1.f, 0.f }, { 0.f, 1.f, 0.f },
		// Bottom (Y-)
		{ 0.f, -1.f, 0.f }, { 0.f, -1.f, 0.f }, { 0.f, -1.f, 0.f },
		{ 0.f, -1.f, 0.f }, { 0.f, -1.f, 0.f }, { 0.f, -1.f, 0.f }
	};

	// Colors
	mesh.colors.resize(36, color);

	return mesh;
}


// 自定义圆柱体
SimpleMeshData make_cylinder(float length, float radius, Vec3f color, std::size_t subdivs = 32)
{
	SimpleMeshData mesh;
	mesh.positions.reserve(subdivs * 6 + subdivs * 6); // shell + caps
	mesh.colors.reserve(subdivs * 6 + subdivs * 6);
	mesh.normals.reserve(subdivs * 6 + subdivs * 6);

	// ---- Cylinder shell ----
	for (std::size_t i = 0; i < subdivs; ++i)
	{
		float a0 = (i) / float(subdivs) * 2.f * std::numbers::pi_v<float>;
		float a1 = (i + 1) / float(subdivs) * 2.f * std::numbers::pi_v<float>;

		float y0 = std::cos(a0) * radius, z0 = std::sin(a0) * radius;
		float y1 = std::cos(a1) * radius, z1 = std::sin(a1) * radius;

		// Normals
		Vec3f n0 = { 0.f, std::cos(a0), std::sin(a0) };  // 径向向
		Vec3f n1 = { 0.f, std::cos(a1), std::sin(a1) };

		// tri 1
		mesh.positions.emplace_back(Vec3f{ 0.f, y0, z0 });
		mesh.positions.emplace_back(Vec3f{ 0.f, y1, z1 });
		mesh.positions.emplace_back(Vec3f{ length, y0, z0 });
		mesh.normals.emplace_back(n0);
		mesh.normals.emplace_back(n1);
		mesh.normals.emplace_back(n0);

		// tri 2
		mesh.positions.emplace_back(Vec3f{ 0.f, y1, z1 });
		mesh.positions.emplace_back(Vec3f{ length, y1, z1 });
		mesh.positions.emplace_back(Vec3f{ length, y0, z0 });
		mesh.normals.emplace_back(n1);
		mesh.normals.emplace_back(n1);
		mesh.normals.emplace_back(n0);

		for (int k = 0; k < 6; ++k) mesh.colors.push_back(color);
	}

	Vec3f center0{ 0.f, 0.f, 0.f };
	Vec3f normalLeft{ -1.f, 0.f, 0.f };
	
	for (std::size_t i = 0; i < subdivs; ++i)
	{
		float a0 = (i) / float(subdivs) * 2.f * std::numbers::pi_v<float>;
		float a1 = (i + 1) / float(subdivs) * 2.f * std::numbers::pi_v<float>;

		float y0 = std::cos(a0) * radius, z0 = std::sin(a0) * radius;
		float y1 = std::cos(a1) * radius, z1 = std::sin(a1) * radius;

		mesh.positions.emplace_back(center0);
		mesh.positions.emplace_back(Vec3f{ 0.f, y1, z1 });
		mesh.positions.emplace_back(Vec3f{ 0.f, y0, z0 });
		
		mesh.normals.emplace_back(normalLeft);
		mesh.normals.emplace_back(normalLeft);
		mesh.normals.emplace_back(normalLeft);

		for (int k = 0; k < 3; ++k) mesh.colors.push_back(color);
	}

	Vec3f center1{ length, 0.f, 0.f };
	Vec3f normalRight{ 1.f, 0.f, 0.f };

	for (std::size_t i = 0; i < subdivs; ++i)
	{
		float a0 = (i) / float(subdivs) * 2.f * std::numbers::pi_v<float>;
		float a1 = (i + 1) / float(subdivs) * 2.f * std::numbers::pi_v<float>;

		float y0 = std::cos(a0) * radius, z0 = std::sin(a0) * radius;
		float y1 = std::cos(a1) * radius, z1 = std::sin(a1) * radius;

		mesh.positions.emplace_back(center1);
		mesh.positions.emplace_back(Vec3f{ length, y0, z0 });
		mesh.positions.emplace_back(Vec3f{ length, y1, z1 });
		
		mesh.normals.emplace_back(normalRight);
		mesh.normals.emplace_back(normalRight);
		mesh.normals.emplace_back(normalRight);

		for (int k = 0; k < 3; ++k) mesh.colors.push_back(color);
	}

	return mesh;
}


 // 创建圆锥体
SimpleMeshData make_cone(float height, float radius, Vec3f color, std::size_t subdivs = 32)
{
	SimpleMeshData mesh;
	mesh.positions.reserve(subdivs * 3 + subdivs * 3); // 侧面 + 底面
	mesh.colors.reserve(subdivs * 3 + subdivs * 3);
	mesh.normals.reserve(subdivs * 3 + subdivs * 3);

	// 顶点和底部中心
	Vec3f apex{0.f, height, 0.f}; 
	Vec3f base_center{0.f, 0.f, 0.f};  

	for (std::size_t i = 0; i < subdivs; ++i)
	{
		float a0 = (i) / float(subdivs) * 2.f * std::numbers::pi_v<float>;
		float a1 = (i + 1) / float(subdivs) * 2.f * std::numbers::pi_v<float>;

		// 底部圆周上的点
		float x0 = std::cos(a0) * radius, z0 = std::sin(a0) * radius;
		float x1 = std::cos(a1) * radius, z1 = std::sin(a1) * radius;

		Vec3f p0{x0, 0.f, z0};
		Vec3f p1{x1, 0.f, z1};

		// 计算侧面法线
		float slope = radius / height;  // 斜率
		Vec3f n0 = {std::cos(a0), slope, std::sin(a0)};
		Vec3f n1 = {std::cos(a1), slope, std::sin(a1)};
		
		// 归一化法线
		n0 = normalize(n0);
		n1 = normalize(n1);

		// 侧面三角形
		mesh.positions.emplace_back(p0);
		mesh.positions.emplace_back(p1);
		mesh.positions.emplace_back(apex);
		mesh.normals.emplace_back(n0);
		mesh.normals.emplace_back(n1);
		mesh.normals.emplace_back(n1);  

		for (int k = 0; k < 3; ++k) mesh.colors.push_back(color);
	}

	Vec3f normal_down{0.f, -1.f, 0.f};
	
	for (std::size_t i = 0; i < subdivs; ++i)
	{
		float a0 = (i) / float(subdivs) * 2.f * std::numbers::pi_v<float>;
		float a1 = (i + 1) / float(subdivs) * 2.f * std::numbers::pi_v<float>;

		float x0 = std::cos(a0) * radius, z0 = std::sin(a0) * radius;
		float x1 = std::cos(a1) * radius, z1 = std::sin(a1) * radius;

		mesh.positions.emplace_back(base_center);
		mesh.positions.emplace_back(Vec3f{x1, 0.f, z1});
		mesh.positions.emplace_back(Vec3f{x0, 0.f, z0});
		
		mesh.normals.emplace_back(normal_down);
		mesh.normals.emplace_back(normal_down);
		mesh.normals.emplace_back(normal_down);

		for (int k = 0; k < 3; ++k) mesh.colors.push_back(color);
	}

	return mesh;
}


ModelMeshData make_tardis(float width, float height, float depth)
{
	ModelMeshData model;
	
	// 材质 0：蓝色（主体和顶部长方体、圆锥）
	MaterialData blue_material;
	blue_material.name = "tardis_blue";
	blue_material.Ka = Vec3f{0.05f, 0.05f, 0.1f};   // 环境光
	blue_material.Kd = tardis_blue;  // 漫反射
	blue_material.Ks = Vec3f{0.2f, 0.2f, 0.3f};     // 镜面反射
	blue_material.Ns = 32.0f;     // 光泽度
	blue_material.illum = 2;
	
	// 材质 1：白色（窗户和圆柱）
	MaterialData white_material;
	white_material.name = "tardis_white";
	white_material.Ka = Vec3f{0.2f, 0.2f, 0.2f}; 
	white_material.Kd = tardis_white;
	white_material.Ks = Vec3f{0.5f, 0.5f, 0.5f};    
	white_material.Ns = 64.0f;      
	white_material.illum = 2;
	
	model.materials.push_back(blue_material);   
	model.materials.push_back(white_material); 
	
	// 1. 主体：蓝色长方体
	SimpleMeshData main_body = make_cuboid(width, height, depth, tardis_blue);
	model.mesh.positions.insert(model.mesh.positions.end(), main_body.positions.begin(), main_body.positions.end());
	model.mesh.colors.insert(model.mesh.colors.end(), main_body.colors.begin(), main_body.colors.end());
	model.mesh.normals.insert(model.mesh.normals.end(), main_body.normals.begin(), main_body.normals.end());
	for (int i = 0; i < 12; ++i) model.triangleMaterialIds.push_back(0);  // 材质 0 (蓝色)
	
	// 2. 窗户参数
	float window_size = width * 0.35f;
	float window_depth = 0.02f;
	float window_gap = 0.05f;
	float offset_from_center = window_size * 0.5f + window_gap * 0.5f;
	float vertical_offset = height * 0.25f;
	
	float window_positions[2][2] = {
		{ -offset_from_center, vertical_offset },
		{ offset_from_center, vertical_offset }
	};
	
	struct FaceConfig {
		float x_offset;
		float z_offset;
		bool is_front_back;
	};
	
	FaceConfig faces[4] = {
		{ 0.f, depth * 0.501f, true },
		{ 0.f, -depth * 0.501f, true },
		{ -width * 0.501f, 0.f, false },
		{ width * 0.501f, 0.f, false }
	};
	
	// 创建 8 个白色窗户
	for (int face = 0; face < 4; ++face)
	{
		for (int win = 0; win < 2; ++win)
		{
			SimpleMeshData window_mesh;
			if (faces[face].is_front_back)
				window_mesh = make_cuboid(window_size, window_size, window_depth, tardis_white);
			else
				window_mesh = make_cuboid(window_depth, window_size, window_size, tardis_white);
			
			for (auto& pos : window_mesh.positions)
			{
				float win_x = window_positions[win][0];
				float win_y = window_positions[win][1];
				if (faces[face].is_front_back)
				{
					pos.x += win_x;
					pos.y += win_y;
					pos.z += faces[face].z_offset;
				}
				else
				{
					pos.x += faces[face].x_offset;
					pos.y += win_y;
					pos.z += win_x;
				}
			}
			
			model.mesh.positions.insert(model.mesh.positions.end(), window_mesh.positions.begin(), window_mesh.positions.end());
			model.mesh.colors.insert(model.mesh.colors.end(), window_mesh.colors.begin(), window_mesh.colors.end());
			model.mesh.normals.insert(model.mesh.normals.end(), window_mesh.normals.begin(), window_mesh.normals.end());
			for (int i = 0; i < 12; ++i) model.triangleMaterialIds.push_back(1);  // 材质 1 (白色)
		}
	}
	
	// 3. 顶部蓝色长方体
	float top_width = width * 0.9f;
	float top_depth = depth * 0.9f;
	float top_height = height / 14.f;
	SimpleMeshData top_box = make_cuboid(top_width, top_height, top_depth, tardis_blue);
	float box_offset_y = height * 0.5f + top_height * 0.5f;
	for (auto& pos : top_box.positions) pos.y += box_offset_y;
	
	model.mesh.positions.insert(model.mesh.positions.end(), top_box.positions.begin(), top_box.positions.end());
	model.mesh.colors.insert(model.mesh.colors.end(), top_box.colors.begin(), top_box.colors.end());
	model.mesh.normals.insert(model.mesh.normals.end(), top_box.normals.begin(), top_box.normals.end());
	for (int i = 0; i < 12; ++i) model.triangleMaterialIds.push_back(0);  // 材质 0 (蓝色)

	float bottom_width = width * 1.1f;
	float bottom_depth = depth * 1.1f;
	float bottom_height = height / 14.f;
	SimpleMeshData bottom_box = make_cuboid(bottom_width, bottom_height, bottom_depth, tardis_blue);
	float bottom_box_offset_y = -height * 0.5f - bottom_height * 0.5f;
	for (auto& pos : bottom_box.positions) pos.y += bottom_box_offset_y;
	
	model.mesh.positions.insert(model.mesh.positions.end(), bottom_box.positions.begin(), bottom_box.positions.end());
	model.mesh.colors.insert(model.mesh.colors.end(), bottom_box.colors.begin(), bottom_box.colors.end());
	model.mesh.normals.insert(model.mesh.normals.end(), bottom_box.normals.begin(), bottom_box.normals.end());
	for (int i = 0; i < 12; ++i) model.triangleMaterialIds.push_back(0);  // 材质 0 (蓝色)
	
	// 4. 白色圆柱
	float cylinder_radius = width * 0.13f;
	float cylinder_height = height * 0.08f;
	SimpleMeshData cylinder_mesh = make_cylinder(cylinder_height, cylinder_radius, tardis_white);
	float cylinder_offset_y = box_offset_y + top_height * 0.5f;
	
	for (auto& pos : cylinder_mesh.positions)
	{
		float temp_x = pos.x, temp_y = pos.y, temp_z = pos.z;
		pos.x = temp_z;
		pos.y = temp_x + cylinder_offset_y;
		pos.z = -temp_y;
	}
	for (auto& normal : cylinder_mesh.normals)
	{
		float temp_x = normal.x, temp_y = normal.y, temp_z = normal.z;
		normal.x = temp_z;
		normal.y = temp_x;
		normal.z = -temp_y;
	}
	
	size_t cylinder_triangles = cylinder_mesh.positions.size() / 3;
	model.mesh.positions.insert(model.mesh.positions.end(), cylinder_mesh.positions.begin(), cylinder_mesh.positions.end());
	model.mesh.colors.insert(model.mesh.colors.end(), cylinder_mesh.colors.begin(), cylinder_mesh.colors.end());
	model.mesh.normals.insert(model.mesh.normals.end(), cylinder_mesh.normals.begin(), cylinder_mesh.normals.end());
	for (size_t i = 0; i < cylinder_triangles; ++i) model.triangleMaterialIds.push_back(1);  // 材质 1 (白色)
	
	// 5. 蓝色圆锥
	float cone_height = height * 0.08f;
	float cone_radius = width * 0.15f;
	SimpleMeshData cone_mesh = make_cone(cone_height, cone_radius, tardis_blue);
	float cone_offset_y = cylinder_offset_y + cylinder_height;
	for (auto& pos : cone_mesh.positions) pos.y += cone_offset_y;
	
	size_t cone_triangles = cone_mesh.positions.size() / 3;
	model.mesh.positions.insert(model.mesh.positions.end(), cone_mesh.positions.begin(), cone_mesh.positions.end());
	model.mesh.colors.insert(model.mesh.colors.end(), cone_mesh.colors.begin(), cone_mesh.colors.end());
	model.mesh.normals.insert(model.mesh.normals.end(), cone_mesh.normals.begin(), cone_mesh.normals.end());
	for (size_t i = 0; i < cone_triangles; ++i) model.triangleMaterialIds.push_back(0);  // 材质 0 (蓝色)
	
	return model;
}