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


//SimpleMeshData make_red_cylinder(std::size_t subdivs = 32)
//{
//	SimpleMeshData mesh;
//	mesh.positions.reserve(subdivs * 6 + subdivs * 6);
//	mesh.colors.reserve(subdivs * 6 + subdivs * 6);
//	mesh.normals.reserve(subdivs * 6 + subdivs * 6);   // 法线
//
//	Vec3f red = { 1.f, 0.f, 0.f };
//
//	 ---- Cylinder shell ----
//	for (std::size_t i = 0; i < subdivs; ++i)
//	{
//		float a0 = (i) / float(subdivs) * 2.f * std::numbers::pi_v<float>;
//		float a1 = (i + 1) / float(subdivs) * 2.f * std::numbers::pi_v<float>;

		//    float y0 = std::cos(a0), z0 = std::sin(a0);
		//    float y1 = std::cos(a1), z1 = std::sin(a1);
//
//		float y0 = std::cos(a0), z0 = std::sin(a0);
//		float y1 = std::cos(a1), z1 = std::sin(a1);
//
//		 Norm
//		Vec3f n0 = { 0.f, y0, z0 };  // 径向向外
//		Vec3f n1 = { 0.f, y1, z1 };
//
//		 tri 1
//		mesh.positions.emplace_back(Vec3f{ 0.f, y0, z0 });
//		mesh.positions.emplace_back(Vec3f{ 0.f, y1, z1 });
//		mesh.positions.emplace_back(Vec3f{ 1.f, y0, z0 });
//		mesh.normals.emplace_back(n0);
//		mesh.normals.emplace_back(n1);
//		mesh.normals.emplace_back(n0);
//
//		 tri 2
//		mesh.positions.emplace_back(Vec3f{ 0.f, y1, z1 });
//		mesh.positions.emplace_back(Vec3f{ 1.f, y1, z1 });
//		mesh.positions.emplace_back(Vec3f{ 1.f, y0, z0 });
//		mesh.normals.emplace_back(n1);
//		mesh.normals.emplace_back(n1);
//		mesh.normals.emplace_back(n0);
//
//		for (int k = 0; k < 6; ++k) mesh.colors.push_back(red);
//	}
//
//	 ---- Cap at x = 0 (normal pointing left: -X) ----
//	Vec3f center0{ 0.f, 0.f, 0.f };
//	Vec3f normalLeft{ -1.f, 0.f, 0.f };
//	
//	for (std::size_t i = 0; i < subdivs; ++i)
//	{
//		float a0 = (i) / float(subdivs) * 2.f * std::numbers::pi_v<float>;
//		float a1 = (i + 1) / float(subdivs) * 2.f * std::numbers::pi_v<float>;

		//    float y0 = std::cos(a0), z0 = std::sin(a0);
		//    float y1 = std::cos(a1), z1 = std::sin(a1);
//
//		mesh.positions.emplace_back(center0);
//		mesh.positions.emplace_back(Vec3f{ 0.f, y1, z1 });
//		mesh.positions.emplace_back(Vec3f{ 0.f, y0, z0 });
//		
//		mesh.normals.emplace_back(normalLeft);
//		mesh.normals.emplace_back(normalLeft);
//		mesh.normals.emplace_back(normalLeft);
//
//		for (int k = 0; k < 3; ++k) mesh colors.push_back(red);
//	}
//
//	Vec3f center1{ 1.f, 0.f, 0.f };
//	Vec3f normalRight{ 1.f, 0.f, 0.f };
//
//	for (std::size_t i = 0; i < subdivs; ++i)
//	{
//		float a0 = (i) / float(subdivs) * 2.f * std::numbers::pi_v<float>;
//		float a1 = (i + 1) / float(subdivs) * 2.f * std::numbers::pi_v<float>;

		//		float y0 = std::cos(a0), z0 = std::sin(a0);
		//		float y1 = std::cos(a1), z1 = std::sin(a1);
//
//		mesh.positions.emplace_back(center1);
//		mesh.positions.emplace_back(Vec3f{ 1.f, y0, z0 });
//		mesh.positions.emplace_back(Vec3f{ 1.f, y1, z1 });
//		
//		mesh.normals.emplace_back(normalRight);
//		mesh.normals.emplace_back(normalRight);
//		mesh.normals.emplace_back(normalRight);
//
//		for (int k = 0; k < 3; ++k) mesh.colors.push_back(red);
//	}
//
//	return mesh;
//}


// 创建可自定义尺寸的长方体
SimpleMeshData make_cuboid(float width, float height, float depth, Vec3f color = {1.f, 1.f, 1.f})
{
	SimpleMeshData mesh;
	
	// 半尺寸
	float hx = width * 0.5f;
	float hy = height * 0.5f;
	float hz = depth * 0.5f;

	// 36 个顶点 (6 faces × 2 triangles × 3 vertices)
	mesh.positions = {
		// Front face (Z+)
		{ -hx, -hy,  hz }, {  hx, -hy,  hz }, {  hx,  hy,  hz },
		{ -hx, -hy,  hz }, {  hx,  hy,  hz }, { -hx,  hy,  hz },
		// Back face (Z-)
		{ -hx, -hy, -hz }, { -hx,  hy, -hz }, {  hx,  hy, -hz },
		{ -hx, -hy, -hz }, {  hx,  hy, -hz }, {  hx, -hy, -hz },
		// Left face (X-)
		{ -hx, -hy, -hz }, { -hx, -hy,  hz }, { -hx,  hy,  hz },
		{ -hx, -hy, -hz }, { -hx,  hy,  hz }, { -hx,  hy, -hz },
		// Right face (X+)
		{  hx, -hy, -hz }, {  hx,  hy, -hz }, {  hx,  hy,  hz },
		{  hx, -hy, -hz }, {hx,  hy,  hz }, {  hx, -hy,  hz },
		// Top face (Y+)
		{ -hx,  hy, -hz }, { -hx,  hy,  hz }, {  hx,  hy,  hz },
		{ -hx,  hy, -hz }, {  hx,  hy,  hz }, {  hx,  hy, -hz },
		// Bottom face (Y-)
		{ -hx, -hy, -hz }, {  hx, -hy, -hz }, {  hx, -hy,  hz },
		{ -hx, -hy, -hz }, {  hx, -hy,  hz }, { -hx, -hy,  hz }
	};

	// 法线
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

	// 统一颜色
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
		float len0 = std::sqrt(n0.x*n0.x + n0.y*n0.y + n0.z*n0.z);
		float len1 = std::sqrt(n1.x*n1.x + n1.y*n1.y + n1.z*n1.z);
		n0.x /= len0; n0.y /= len0; n0.z /= len0;
		n1.x /= len1; n1.y /= len1; n1.z /= len1;

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


SimpleMeshData make_tardis(float width, float height, float depth)
{
	// 主体：蓝色长方体
	SimpleMeshData mesh = make_cuboid(width, height, depth, tardis_blue);
	
	// 窗户参数
	float window_size = width * 0.35f; // 窗户尺寸
	float window_depth = 0.02f; // 窗户厚度很小
	float window_gap = 0.05f; // 窗户间距
	
	// 计算窗户在每个面上的位置
	float offset_from_center = window_size * 0.5f + window_gap * 0.5f;
	float vertical_offset = height * 0.25f;  // 向上偏移更多，让窗户在上方
	
	// 只有2个窗户位置（左上、右上）
	float window_positions[2][2] = {
		{ -offset_from_center, vertical_offset },   // 左上
		{ offset_from_center, vertical_offset }     // 右上
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
	
	// 为每个面创建 2 个窗户
	for (int face = 0; face < 4; ++face)
	{
		for (int win = 0; win < 2; ++win)  
		{
			SimpleMeshData window_mesh;
			
			if (faces[face].is_front_back)
			{

				window_mesh = make_cuboid(window_size, window_size, window_depth, tardis_white);
			}
			else
			{
				window_mesh = make_cuboid(window_depth, window_size, window_size, tardis_white);
			}
			
			// 根据面的方向调整窗户位置
			for (auto& pos : window_mesh.positions)
			{
				float win_x = window_positions[win][0];
				float win_y = window_positions[win][1];
				
				if (faces[face].is_front_back)
				{
					// 前面和后面：x 和 y 方向的偏移
					pos.x += win_x;
					pos.y += win_y;
					pos.z += faces[face].z_offset;
				}
				else
				{
					// 左面和右面：z 和 y 方向的偏移
					pos.x += faces[face].x_offset;
					pos.y += win_y;
					pos.z += win_x; 
				}
			}
			
			// 合并窗户网格到主网格
			mesh.positions.insert(mesh.positions.end(), window_mesh.positions.begin(), window_mesh.positions.end());
			mesh.colors.insert(mesh.colors.end(), window_mesh.colors.begin(), window_mesh.colors.end());
			mesh.normals.insert(mesh.normals.end(), window_mesh.normals.begin(), window_mesh.normals.end());
		}
	}
	
	float top_width = width * 0.9f;   
	float top_depth = depth * 0.9f;   
	float top_height = height / 14.f; 
	SimpleMeshData top_box = make_cuboid(top_width, top_height, top_depth, tardis_blue);
	
	// 将第一层长方体放置在主体顶部
	float box_offset_y = height * 0.5f + top_height * 0.5f;  // 主体顶部 + 长方体中心偏移
	
	for (auto& pos : top_box.positions)
	{
		pos.y += box_offset_y;  // 只需要 Y 方向偏移
	}
	
	// 合并第一层长方体到主网格
	mesh.positions.insert(mesh.positions.end(), top_box.positions.begin(), top_box.positions.end());
	mesh.colors.insert(mesh.colors.end(), top_box.colors.begin(), top_box.colors.end());
	mesh.normals.insert(mesh.normals.end(), top_box.normals.begin(), top_box.normals.end());
	
	// 第二层顶部：白色圆柱体（放在长方体顶部）
	float cylinder_radius = width * 0.13f;  // 圆柱半径
	float cylinder_height = height * 0.08f;  // 圆柱高度
	SimpleMeshData cylinder_mesh = make_cylinder(cylinder_height, cylinder_radius, tardis_white);
	
	// 将圆柱体放置在第一层长方体的顶部
	float cylinder_offset_y = box_offset_y + top_height * 0.5f;  // 长方体顶部位置
	
	for (auto& pos : cylinder_mesh.positions)
	{
		// 旋转：X轴圆柱 -> Y轴圆柱
		float temp_x = pos.x;
		float temp_y = pos.y;
		float temp_z = pos.z;
		
		pos.x = temp_z;
		pos.y = temp_x + cylinder_offset_y;
		pos.z = -temp_y;
	}
	
	// 旋转法线
	for (auto& normal : cylinder_mesh.normals)
	{
		float temp_x = normal.x;
		float temp_y = normal.y;
		float temp_z = normal.z;
		
		normal.x = temp_z;
		normal.y = temp_x;
		normal.z = -temp_y;
	}
	
	// 合并圆柱体网格到主网格
	mesh.positions.insert(mesh.positions.end(), cylinder_mesh.positions.begin(), cylinder_mesh.positions.end());
	mesh.colors.insert(mesh.colors.end(), cylinder_mesh.colors.begin(), cylinder_mesh.colors.end());
	mesh.normals.insert(mesh.normals.end(), cylinder_mesh.normals.begin(), cylinder_mesh.normals.end());
	
	// 第三层顶部：白色圆锥体（放在圆柱顶部，作为天线/灯）
	float cone_height = height * 0.08f;   // 圆锥高度
	float cone_radius = width * 0.15f;    // 圆锥底部半径（略小于圆柱）
	SimpleMeshData cone_mesh = make_cone(cone_height, cone_radius, tardis_blue);
	
	// 将圆锥体放置在圆柱的顶部
	float cone_offset_y = cylinder_offset_y + cylinder_height;  // 圆柱顶部位置
	
	for (auto& pos : cone_mesh.positions)
	{
		pos.y += cone_offset_y; 
	}
	
	// 合并圆锥体网格到主网格
	mesh.positions.insert(mesh.positions.end(), cone_mesh.positions.begin(), cone_mesh.positions.end());
	mesh.colors.insert(mesh.colors.end(), cone_mesh.colors.begin(), cone_mesh.colors.end());
	mesh.normals.insert(mesh.normals.end(), cone_mesh.normals.begin(), cone_mesh.normals.end());
	
	return mesh;
}