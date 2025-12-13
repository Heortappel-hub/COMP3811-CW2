#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <print>
#include <numbers>
#include <typeinfo>
#include <stdexcept>
#include <cmath>

#include <cstdlib>

#include "../support/error.hpp"
#include "../support/program.hpp"
#include "../support/checkpoint.hpp"
#include "../support/debug_output.hpp"

#include "../vmlib/vec4.hpp"
#include "../vmlib/mat44.hpp"

#include "defaults.hpp" // map shapder

#include "simple_mesh.hpp" // 简单网格
#include "loadobj.hpp" // 加载 OBJ 模型
#include "texture.hpp"   // 贴图
#include "Custom_model.hpp"  // 自定义模型

#include <rapidobj/rapidobj.hpp>




namespace
{
	constexpr char const* kWindowTitle = "COMP3811 - CW2";
	
	constexpr float kPi_ = 3.1415926f;
	constexpr float kMovementSpeed = 10.f;
	constexpr float kMouseSensitivity = 0.01f;

	struct CamCtrl_
	{
		bool cameraActive = false;
		bool forward = false, back = false, left = false, right = false;
		bool up = false, down = false;
		bool shiftPressed = false;  // 追踪 Shift 键状态
		bool ctrlPressed = false;   // 追踪 Ctrl 键状态
		
		// 光源开关
		bool enablePointLight1 = true;   
		bool enablePointLight2 = true;   
		bool enablePointLight3 = true;   
		bool enableDirectionalLight = true;
		
		// 动画状态
		bool animationActive = false;    // 动画是否激活
		bool animationPaused = false;    // 动画是否暂停
		float animationTime = 0.f;    // 动画时间
		
		float phi = 0.f, theta = 0.f;
		float posX = 100.f, posY = 10.f, posZ = 100.f; 
		float lastX = 0.f, lastY = 0.f;


	};
	
	void glfw_callback_error_( int, char const* );
	void glfw_callback_key_( GLFWwindow*, int, int, int, int );
	void glfw_callback_motion_( GLFWwindow*, double, double );
	void glfw_callback_mouse_button_( GLFWwindow*, int, int, int );  

	struct GLFWCleanupHelper
	{
		~GLFWCleanupHelper();
	};
	struct GLFWWindowDeleter
	{
		~GLFWWindowDeleter();
		GLFWwindow* window;
	};
}

int main() try
{
	// Initialize GLFW
	if( GLFW_TRUE != glfwInit() )
	{
		char const* msg = nullptr;
		int ecode = glfwGetError( &msg );
		throw Error( "glfwInit() failed with '{}' ({})", msg, ecode );
	}

	// Ensure that we call glfwTerminate() at the end of the program.
	GLFWCleanupHelper cleanupHelper;

	// Configure GLFW and create window
	glfwSetErrorCallback( &glfw_callback_error_ );

	glfwWindowHint( GLFW_SRGB_CAPABLE, GLFW_TRUE );
	glfwWindowHint( GLFW_DOUBLEBUFFER, GLFW_TRUE );

	//glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );

	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
	glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

	glfwWindowHint( GLFW_DEPTH_BITS, 24 );

#	if !defined(NDEBUG)
	// When building in debug mode, request an OpenGL debug context.
	// Enables additional debugging features. However, this can carry extra
	// overheads. We therefore do not do this for release builds.
	glfwWindowHint( GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE );
#	endif // ~ !NDEBUG

	GLFWwindow* window = glfwCreateWindow(
		1280,
		720,
		kWindowTitle,
		nullptr, nullptr
	);

	if( !window )
	{
		char const* msg = nullptr;
		int ecode = glfwGetError( &msg );
		throw Error( "glfwCreateWindow() failed with '{}' ({})", msg, ecode );
	}

	GLFWWindowDeleter windowDeleter{ window };

	// Camera control state
	CamCtrl_ camControl;

	// Set up event handling
	glfwSetWindowUserPointer(window, &camControl);
	glfwSetKeyCallback( window, &glfw_callback_key_ );
	glfwSetCursorPosCallback( window, &glfw_callback_motion_ );
	glfwSetMouseButtonCallback( window, &glfw_callback_mouse_button_ ); 

	// Set up drawing stuff
	glfwMakeContextCurrent( window );
	glfwSwapInterval( 1 ); 


	// Initialize GLAD
	// This will load the OpenGL API. We mustn't make any OpenGL calls before this!
	if( !gladLoadGLLoader( (GLADloadproc)&glfwGetProcAddress ) )
		throw Error( "gladLoadGLLoader() failed - cannot load GL API!" );

	std::print( "RENDERER {}\n", (char const*)glGetString( GL_RENDERER ) );
	std::print( "VENDOR {}\n", (char const*)glGetString( GL_VENDOR ) );
	std::print( "VERSION {}\n", (char const*)glGetString( GL_VERSION ) );
	std::print( "SHADING_LANGUAGE_VERSION {}\n", (char const*)glGetString( GL_SHADING_LANGUAGE_VERSION ) );

	// Ddebug output
#	if !defined(NDEBUG)
	setup_gl_debug_output();
#	endif // ~ !NDEBUG

	// Global GL state
	OGL_CHECKPOINT_ALWAYS();
	glEnable( GL_FRAMEBUFFER_SRGB );
	glEnable( GL_DEPTH_TEST );
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	OGL_CHECKPOINT_ALWAYS();

	// Get actual framebuffer size.
	int iwidth, iheight;
	glfwGetFramebufferSize( window, &iwidth, &iheight );

	glViewport( 0, 0, iwidth, iheight );


	OGL_CHECKPOINT_ALWAYS();

	// Build shader program
	ShaderProgram prog_map({
		ShaderProgram::ShaderSource{ GL_VERTEX_SHADER, "./assets/cw2/default.vert" },
		ShaderProgram::ShaderSource{ GL_FRAGMENT_SHADER,"./assets/cw2/default.frag" }
	});

	// Blinn-Phong shader
	ShaderProgram prog_blinn_phong({
		ShaderProgram::ShaderSource{ GL_VERTEX_SHADER, "./assets/cw2/blinn_phong.vert" },
		ShaderProgram::ShaderSource{ GL_FRAGMENT_SHADER, "./assets/cw2/blinn_phong.frag" }
	});

	// shader for custom models
	ShaderProgram prog_color({
		ShaderProgram::ShaderSource{ GL_VERTEX_SHADER, "./assets/cw2/color.vert" },
		ShaderProgram::ShaderSource{ GL_FRAGMENT_SHADER, "./assets/cw2/color.frag" }
	});

	// Load the mesh
	ModelMeshData parlahti_model = load_wavefront_obj_mat( "./assets/cw2/parlahti.obj" );
	ModelMeshData landingpad_model = load_wavefront_obj_mat( "./assets/cw2/landingpad.obj" );
	ModelMeshData landingpad_model_1 = load_wavefront_obj_mat("./assets/cw2/landingpad.obj");
	
	std::print("=== OBJ Loading Info:Parlahti ===\n");
	std::print("Positions: {}\n", parlahti_model.mesh.positions.size());
	std::print("Normals: {}\n", parlahti_model.mesh.normals.size());
	std::print("Texcoords: {}\n", parlahti_model.mesh.texcoords.size());
	std::print("Materials: {}\n", parlahti_model.materials.size());
	std::print("Triangle Material IDs: {}\n", parlahti_model.triangleMaterialIds.size());
	
	std::print("=== OBJ Loading Info:Landingpad ===\n");
	std::print("Positions: {}\n", landingpad_model.mesh.positions.size());
	std::print("Normals: {}\n", landingpad_model.mesh.normals.size());
	std::print("Texcoords: {}\n", landingpad_model.mesh.texcoords.size());
	std::print("Materials: {}\n", landingpad_model.materials.size());
	std::print("Triangle Material IDs: {}\n", landingpad_model.triangleMaterialIds.size());


	if (parlahti_model.mesh.positions.empty())
		throw Error("OBJ parlahti has no positions");
	else if(landingpad_model.mesh.positions.empty())
		throw Error("OBJ landingpad has no positions");
	
	
	
	// Upload mesh to GPU
	GLuint parlahti_vao = create_vao_mat(parlahti_model);
	GLsizei vertexCount = static_cast<GLsizei>( parlahti_model.mesh.positions.size() );


	std::print("Vertex count for map rendering: {}\n", vertexCount);

	GLuint landingpad_vao = create_vao_mat(landingpad_model);
	GLsizei landingpad_vertexCount = static_cast<GLsizei>(landingpad_model.mesh.positions.size());

	std::print("Vertex count for landingpad rendering: {}\n", landingpad_vertexCount);

	GLuint landingpad_vao_1 = create_vao_mat(landingpad_model_1);
	GLsizei landingpad_vertexCount_1 = static_cast<GLsizei>(landingpad_model_1.mesh.positions.size());

	std::print("Vertex count for landingpad rendering: {}\n", landingpad_vertexCount_1);


	// Load texture
	GLuint parlahti_Texture = load_texture_2d("./assets/cw2/L4343A-4k.jpeg");
	std::print("Texture loaded successfully\n");

	// Create ship with material
	ModelMeshData tardis_model = make_tardis(2.0f, 4.0f, 2.0f);
	GLuint tardis_vao = create_vao_mat(tardis_model);
	GLsizei tardis_vertexCount = static_cast<GLsizei>(tardis_model.mesh.positions.size());
	std::print("Vertex count for TARDIS rendering: {}\n", tardis_vertexCount);

	// Setup matrices
	Mat44f proj = make_perspective_projection(
		60.f * std::numbers::pi_v<float> / 180.f, 
		float(iwidth) / float(iheight),
		0.1f,
		1000.f
	);
	Mat44f view;
	Mat44f modelM = kIdentity44f;
	Mat44f uProjCameraWorld;

	// Normal matrix3x3
	float uNormalMatrix[9] = {
		1.f, 0.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 0.f, 1.f
	};


	Vec3f light_nor = { 0.f, 1.f, -1.f };
	light_nor = normalize(light_nor);
	
	// Light direction (world space) - normalized
	float lightDir[3] = { light_nor.x, light_nor.y, light_nor.z };

	// 点光源1：红色
	float pointLight1Pos[3] = {21.5f, 2.f, 15.0f}; 
	float pointLight1Color[3] = {20.0f, 0.0f, 0.0f};  
	
	// 点光源2：绿色
	float pointLight2Pos[3] = {19.5f, 2.f, 18.5f}; 
	float pointLight2Color[3] = {0.0f, 20.0f, 0.0f}; 
	
	// 点光源3：蓝色
	float pointLight3Pos[3] = {23.5f, 2.f, 18.5f}; 
	float pointLight3Color[3] = {0.0f, 0.0f, 20.0f}; 

	// Model transform
	// 位置变换矩阵
	Mat44f map2world = make_translation({ 0.f, 0.f, 0.f });
	Mat44f landingpad2world = make_translation({ 21.5f, -1.0f, 17.f }) * make_scaling(7.f, 7.f, 7.f);
	Mat44f landingpad2world_1 = make_translation({ 50.f, -1.f, 50.f }) * make_scaling(6.f, 6.f, 6.f); 
	
	// 飞船初始位置：与第一个发射台中心对齐
	Vec3f tardisInitialPosition{ 21.5f, 1.35f, 17.f };
	Mat44f tardis2world;  // 将在循环中动态更新

	OGL_CHECKPOINT_ALWAYS();

	// Time tracking for smooth camera movement
	auto lastTime = Clock::now();

	// Main loop
	while( !glfwWindowShouldClose( window ) )
	{
		// Let GLFW process events
		glfwPollEvents();
		
		// Delta time for frame-independent movement
		auto now = Clock::now();
		float dt = std::chrono::duration_cast<Secondsf>(now - lastTime).count();
		lastTime = now;
		
		if (dt <= 0.f || dt > 1.f) {
			dt = 0.016f;  // 默认 60 FPS
		}

		if (camControl.cameraActive)
		{
			// 速度调节：Shift 加速 3 倍，Ctrl 减速到 1/3
			float speedMultiplier = 1.0f;
			if (camControl.shiftPressed)
				speedMultiplier = 3.0f;      // Shift: 3x 速度
			else if (camControl.ctrlPressed)
				speedMultiplier = 1.0f / 3.0f; // Ctrl: 1/3x 速度
			
			float moveSpeed = kMovementSpeed * dt * speedMultiplier;

			// Forward/Back movement
			if (camControl.forward)
			{
				camControl.posZ += moveSpeed * std::cos(camControl.phi);
				camControl.posX -= moveSpeed * std::sin(camControl.phi);
			}
			if (camControl.back)
			{
				camControl.posZ -= moveSpeed * std::cos(camControl.phi);
				camControl.posX += moveSpeed * std::sin(camControl.phi);
			}
			

			// Left/Right
			if (camControl.left)
			{
				camControl.posZ += moveSpeed * std::sin(camControl.phi);
				camControl.posX += moveSpeed * std::cos(camControl.phi);
			}
			if (camControl.right)
			{
				camControl.posZ -= moveSpeed * std::sin(camControl.phi);
				camControl.posX -= moveSpeed * std::cos(camControl.phi);
			}
			

			// Up/Down movement 
			if (camControl.up)
				camControl.posY += moveSpeed;
			if (camControl.down)
				camControl.posY -= moveSpeed;
		}
		
		// Compute view matrix from camera state
		Mat44f Rx = make_rotation_x(camControl.theta);
		Mat44f Ry = make_rotation_y(camControl.phi);
		Mat44f T = make_translation({-camControl.posX, -camControl.posY, -camControl.posZ});
		view = Rx * Ry * T;
		
		// Update combined matrix
		uProjCameraWorld = proj * view * modelM;
		

		// Animetion update
		Vec3f tardisPosition = tardisInitialPosition;  // 初始位置
		float tardisRotationY = 0.f;  // 飞船朝向角度
		
		if (camControl.animationActive && !camControl.animationPaused) {
			float animSpeed = 0.15f * dt;  // 动画速度
			camControl.animationTime += animSpeed;
			
			// 循环动画
			if (camControl.animationTime > 1.f) {
				camControl.animationTime = 0.f;
			}
			
			if (camControl.animationTime < 0.f) {
				camControl.animationTime = 0.f;
			}
			
			// 慢启动 -> 加速
			float t = camControl.animationTime;
			float easedT = t * t * (3.f - 2.f * t);  // Smoothstep
			
			if (easedT < 0.f) easedT = 0.f;
			if (easedT > 1.f) easedT = 1.f;
			
			// 螺旋上升路径参数
			float radius = 15.f;  // 螺旋半径
			float centerX = tardisInitialPosition.x;  
			float centerZ = tardisInitialPosition.z; 
			float heightGain = 30.f;  // 总上升高度
			

			float currentRadius = radius * easedT;  // 螺旋半径随时间增长（从0到15）
			
			// 转 3 圈
			float angle = easedT * 6.f * kPi_;  
			
			if (!std::isfinite(angle)) {
				angle = 0.f;
			}
			
			// 螺旋上升轨迹
			tardisPosition.x = centerX + currentRadius * std::cos(angle);
			tardisPosition.z = centerZ + currentRadius * std::sin(angle);
			tardisPosition.y = tardisInitialPosition.y + heightGain * easedT;
			
			
			if (!std::isfinite(tardisPosition.x) || !std::isfinite(tardisPosition.y) || !std::isfinite(tardisPosition.z)) {
				tardisPosition = tardisInitialPosition;  // 回退到初始位置
			}
			
			// 切线方向：速度的方向就是运动轨迹的切线
			tardisRotationY = angle + kPi_ / 2.f;  // 让飞船朝向运动方向
			

		}
		
		// 更新点光源位置，使其围绕飞船旋转
		float lightRadius = 2.5f;
		
		// 点光源1：红色 (0度位置)
		pointLight1Pos[0] = tardisPosition.x + lightRadius * std::cos(kPi_ / 2.f);
		pointLight1Pos[1] = tardisPosition.y + 0.65f;
		pointLight1Pos[2] = tardisPosition.z + lightRadius * std::sin(kPi_ / 2.f);
		
		// 点光源2：绿色 (120度位置)
		float angle2 = kPi_ / 2.f + 2.0f * kPi_ / 3.0f;
		pointLight2Pos[0] = tardisPosition.x + lightRadius * std::cos(angle2);
		pointLight2Pos[1] = tardisPosition.y + 0.65f;
		pointLight2Pos[2] = tardisPosition.z + lightRadius * std::sin(angle2);
		
		// 点光源3：蓝色 (240度位置)
		float angle3 = kPi_ / 2.f + 4.0f * kPi_ / 3.0f;
		pointLight3Pos[0] = tardisPosition.x + lightRadius * std::cos(angle3);
		pointLight3Pos[1] = tardisPosition.y + 0.65f;
		pointLight3Pos[2] = tardisPosition.z + lightRadius * std::sin(angle3);
		
		// 更新飞船变换矩阵：先旋转再平移（让飞船朝向运动方向）
		tardis2world = make_translation(tardisPosition) * make_rotation_y(tardisRotationY);

		// Check if window was resized
		{
			int nwidth, nheight;
			glfwGetFramebufferSize( window, &nwidth, &nheight );

			if( 0 == nwidth || 0 == nheight )
			{
				// Window minimized - wait until restored
				do
				{
					glfwWaitEvents();
					glfwGetFramebufferSize( window, &nwidth, &nheight );
				} while( 0 == nwidth || 0 == nheight );
			}

			glViewport( 0, 0, nwidth, nheight );
			
			// Update projection if aspect ratio changed
			if (nwidth != iwidth || nheight != iheight)
			{
				iwidth = nwidth;
				iheight = nheight;
				proj = make_perspective_projection(
					60.f * std::numbers::pi_v<float> / 180.f,
					float(iwidth) / float(iheight),
					0.1f,
					1000.f
				);
			}
		}

		// Draw scene
		OGL_CHECKPOINT_DEBUG();
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		glUseProgram( prog_map.programId() );

		Mat44f modelView = view * map2world;
		Mat44f normalMat4 = transpose(modelView);
		
		// Extract 3x3 part
		uNormalMatrix[0] = normalMat4.v[0]; uNormalMatrix[1] = normalMat4.v[1]; uNormalMatrix[2] = normalMat4.v[2];
		uNormalMatrix[3] = normalMat4.v[4]; uNormalMatrix[4] = normalMat4.v[5]; uNormalMatrix[5] = normalMat4.v[6];
		uNormalMatrix[6] = normalMat4.v[8]; uNormalMatrix[7] = normalMat4.v[9]; uNormalMatrix[8] = normalMat4.v[10];

		// Upload uniforms
		glUniformMatrix4fv(0, 1, GL_TRUE, (uProjCameraWorld * map2world).v );
		glUniformMatrix3fv(1, 1, GL_TRUE, uNormalMatrix );
		glUniform3fv(2, 1, lightDir );

		// Bind texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, parlahti_Texture);
		glUniform1i(3, 0);  // Tell shader texture is in unit 0

		// Draw terrain (parlahti)
		glBindVertexArray(parlahti_vao);
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);
		glBindVertexArray(0);

		// Draw landingpad with Blinn-Phong shader
		glUseProgram(prog_blinn_phong.programId());
		
		// Update model-view matrix for landingpad
		Mat44f landingpadModelView = view * landingpad2world;
		Mat44f landingpadNormalMat4 = transpose(landingpadModelView);
		
		// Extract 3x3 part for landingpad normal matrix
		uNormalMatrix[0] = landingpadNormalMat4.v[0]; uNormalMatrix[1] = landingpadNormalMat4.v[1]; uNormalMatrix[2] = landingpadNormalMat4.v[2];
		uNormalMatrix[3] = landingpadNormalMat4.v[4]; uNormalMatrix[4] = landingpadNormalMat4.v[5]; uNormalMatrix[5] = landingpadNormalMat4.v[6];
		uNormalMatrix[6] = landingpadNormalMat4.v[8]; uNormalMatrix[7] = landingpadNormalMat4.v[9]; uNormalMatrix[8] = landingpadNormalMat4.v[10];

		// Upload landingpad uniforms
		glUniformMatrix4fv(0, 1, GL_TRUE, (proj * view * landingpad2world).v);
		glUniformMatrix4fv(14, 1, GL_TRUE, landingpad2world.v); 
		glUniformMatrix3fv(1, 1, GL_TRUE, uNormalMatrix);
		glUniform3fv(2, 1, lightDir);
		
		// Upload camera position for specular calculation
		float cameraPos[3] = {camControl.posX, camControl.posY, camControl.posZ};
		glUniform3fv(3, 1, cameraPos);
		
		// Upload point lights
		glUniform3fv(4, 1, pointLight1Pos);
		glUniform3fv(5, 1, pointLight1Color);
		glUniform3fv(6, 1, pointLight2Pos);
		glUniform3fv(7, 1, pointLight2Color);
		glUniform3fv(8, 1, pointLight3Pos);
		glUniform3fv(9, 1, pointLight3Color);
		
		// Upload light states
		glUniform1i(10, camControl.enablePointLight1 ? 1 : 0);
		glUniform1i(11, camControl.enablePointLight2 ? 1 : 0);
		glUniform1i(12, camControl.enablePointLight3 ? 1 : 0);
		glUniform1i(13, camControl.enableDirectionalLight ? 1 : 0);
		
		// Draw landingpad
		glBindVertexArray(landingpad_vao);
		glDrawArrays(GL_TRIANGLES, 0, landingpad_vertexCount);
		glBindVertexArray(0);

		// Second landingpad
		glUseProgram(prog_blinn_phong.programId());
		
		// Update model-view matrix for second landingpad
		Mat44f landingpadModelView_1 = view * landingpad2world_1;
		Mat44f landingpadNormalMat4_1 = transpose(landingpadModelView_1);
		
		uNormalMatrix[0] = landingpadNormalMat4_1.v[0]; uNormalMatrix[1] = landingpadNormalMat4_1.v[1]; uNormalMatrix[2] = landingpadNormalMat4_1.v[2];
		uNormalMatrix[3] = landingpadNormalMat4_1.v[4]; uNormalMatrix[4] = landingpadNormalMat4_1.v[5]; uNormalMatrix[5] = landingpadNormalMat4_1.v[6];
		uNormalMatrix[6] = landingpadNormalMat4_1.v[8]; uNormalMatrix[7] = landingpadNormalMat4_1.v[9]; uNormalMatrix[8] = landingpadNormalMat4_1.v[10];

		// Upload second landingpad uniforms
		glUniformMatrix4fv(0, 1, GL_TRUE, (proj * view * landingpad2world_1).v);
		glUniformMatrix4fv(14, 1, GL_TRUE, landingpad2world_1.v);  
		glUniformMatrix3fv(1, 1, GL_TRUE, uNormalMatrix);
		glUniform3fv(2, 1, lightDir);
		glUniform3fv(3, 1, cameraPos);
		
		// Upload point lights
		glUniform3fv(4, 1, pointLight1Pos);
		glUniform3fv(5, 1, pointLight1Color);
		glUniform3fv(6, 1, pointLight2Pos);
		glUniform3fv(7, 1, pointLight2Color);
		glUniform3fv(8, 1, pointLight3Pos);
		glUniform3fv(9, 1, pointLight3Color);
		
		// Upload light enable states
		glUniform1i(10, camControl.enablePointLight1 ? 1 : 0);
		glUniform1i(11, camControl.enablePointLight2 ? 1 : 0);
		glUniform1i(12, camControl.enablePointLight3 ? 1 : 0);
		glUniform1i(13, camControl.enableDirectionalLight ? 1 : 0);
		
		// Draw second landingpad
		glBindVertexArray(landingpad_vao_1);
		glDrawArrays(GL_TRIANGLES, 0, landingpad_vertexCount_1);
		glBindVertexArray(0);

		// Draw TARDIS with Blinn-Phong shader
		glUseProgram(prog_blinn_phong.programId());
		
		// Update model-view matrix for TARDIS
		Mat44f tardisModelView = view * tardis2world;
		Mat44f tardisNormalMat4 = transpose(tardisModelView);
		
		// Extract 3x3 part for TARDIS normal matrix
		uNormalMatrix[0] = tardisNormalMat4.v[0]; uNormalMatrix[1] = tardisNormalMat4.v[1]; uNormalMatrix[2] = tardisNormalMat4.v[2];
		uNormalMatrix[3] = tardisNormalMat4.v[4]; uNormalMatrix[4] = tardisNormalMat4.v[5]; uNormalMatrix[5] = tardisNormalMat4.v[6];
		uNormalMatrix[6] = tardisNormalMat4.v[8]; uNormalMatrix[7] = tardisNormalMat4.v[9]; uNormalMatrix[8] = tardisNormalMat4.v[10];

		// Upload ship uniforms
		glUniformMatrix4fv(0, 1, GL_TRUE, (proj * view * tardis2world).v);
		glUniformMatrix4fv(14, 1, GL_TRUE, tardis2world.v);
		glUniformMatrix3fv(1, 1, GL_TRUE, uNormalMatrix);
		glUniform3fv(2, 1, lightDir);
		glUniform3fv(3, 1, cameraPos);
		
		// Upload point lights
		glUniform3fv(4, 1, pointLight1Pos);
		glUniform3fv(5, 1, pointLight1Color);
		glUniform3fv(6, 1, pointLight2Pos);
		glUniform3fv(7, 1, pointLight2Color);
		glUniform3fv(8, 1, pointLight3Pos);
		glUniform3fv(9, 1, pointLight3Color);
		
		// Upload light enable states
		glUniform1i(10, camControl.enablePointLight1 ? 1 : 0);
		glUniform1i(11, camControl.enablePointLight2 ? 1 : 0);
		glUniform1i(12, camControl.enablePointLight3 ? 1 : 0);
		glUniform1i(13, camControl.enableDirectionalLight ? 1 : 0);
		
		// Draw TARDIS
		glBindVertexArray(tardis_vao);
		glDrawArrays(GL_TRIANGLES, 0, tardis_vertexCount);
		glBindVertexArray(0);

		OGL_CHECKPOINT_DEBUG();

		// Display results
		glfwSwapBuffers( window );
	}

	// Cleanup
	return 0;
}

catch( std::exception const& eErr )
{
	std::print( stderr, "Top-level Exception ({}):\n", typeid(eErr).name() );
	std::print( stderr, "{}\n", eErr.what() );
	std::print( stderr, "Bye.\n" );
	return 1;
}


namespace
{
	void glfw_callback_error_( int aErrNum, char const* aErrDesc )
	{
		std::print( stderr, "GLFW error: {} ({})\n", aErrDesc, aErrNum );
	}

	void glfw_callback_key_( GLFWwindow* aWindow, int aKey, int, int aAction, int )
	{
		if( GLFW_KEY_ESCAPE == aKey && GLFW_PRESS == aAction )
		{
			glfwSetWindowShouldClose( aWindow, GLFW_TRUE );
			return;
		}

		auto* cam = static_cast<CamCtrl_*>(glfwGetWindowUserPointer(aWindow));
		if (!cam) return;

		// 光源开关
		if (aAction == GLFW_PRESS) {
			if (GLFW_KEY_1 == aKey) {
				cam->enablePointLight1 = !cam->enablePointLight1;
				std::print("Point Light 1: {}\n", cam->enablePointLight1 ? "ON" : "OFF");
			}
			else if (GLFW_KEY_2 == aKey) {
				cam->enablePointLight2 = !cam->enablePointLight2;
				std::print("Point Light 2: {}\n", cam->enablePointLight2 ? "ON" : "OFF");
			}
			else if (GLFW_KEY_3 == aKey) {
				cam->enablePointLight3 = !cam->enablePointLight3;
				std::print("Point Light 3: {}\n", cam->enablePointLight3 ? "ON" : "OFF");
			}
			else if (GLFW_KEY_4 == aKey) {
				cam->enableDirectionalLight = !cam->enableDirectionalLight;
				std::print("Directional Light: {}\n", cam->enableDirectionalLight ? "ON" : "OFF");
			}
			// F 键：开始/暂停动画
			else if (GLFW_KEY_F == aKey) {
				if (!cam->animationActive) {
					// 按 F：开始动画
					cam->animationActive = true;
					cam->animationPaused = false;
					cam->animationTime = 0.f;
					std::print("Animation STARTED (time = {})\n", cam->animationTime);
				} else {
					// 后续按 F：切换暂停状态
					cam->animationPaused = !cam->animationPaused;
					std::print("Animation {} (time = {})\n", 
						cam->animationPaused ? "PAUSED" : "RESUMED", 
						cam->animationTime);
				}
			}
			// R 键：重置动画
			else if (GLFW_KEY_R == aKey) {
				cam->animationActive = false;
				cam->animationPaused = false;
				cam->animationTime = 0.f;
				std::print("Animation RESET\n");
			}
		}

		// Movement keys (only respond when camera is active)
		if (cam->cameraActive)
		{
			bool isPress = (aAction == GLFW_PRESS);
			bool isRelease = (aAction == GLFW_RELEASE);

			// Handle Shift key for speed boost
			if (GLFW_KEY_LEFT_SHIFT == aKey || GLFW_KEY_RIGHT_SHIFT == aKey)
			{
				if (isPress)
					cam->shiftPressed = true;
				else if (isRelease)
					cam->shiftPressed = false;
			}
			// Handle Ctrl key for slow motion
			else if (GLFW_KEY_LEFT_CONTROL == aKey || GLFW_KEY_RIGHT_CONTROL == aKey)
			{
				if (isPress)
					cam->ctrlPressed = true;  
				else if (isRelease)
					cam->ctrlPressed = false;
			}
			// Movement keys
			else if (GLFW_KEY_S == aKey)
				cam->forward = isPress ? true : (isRelease ? false : cam->forward);
			else if (GLFW_KEY_W == aKey)
				cam->back = isPress ? true : (isRelease ? false : cam->back);
			else if (GLFW_KEY_D == aKey)
				cam->left = isPress ? true : (isRelease ? false : cam->left);
			else if (GLFW_KEY_A == aKey)
				cam->right = isPress ? true : (isRelease ? false : cam->right);
			else if (GLFW_KEY_E == aKey)
				cam->up = isPress ? true : (isRelease ? false : cam->up);
			else if (GLFW_KEY_Q == aKey)
				cam->down = isPress ? true : (isRelease ? false : cam->down);
		}
	}

	void glfw_callback_motion_( GLFWwindow* aWindow, double aX, double aY )
	{
		auto* cam = static_cast<CamCtrl_*>(glfwGetWindowUserPointer(aWindow));
		if (!cam) return;

		if (cam->cameraActive)
		{
			float dx = float(aX - cam->lastX);
			float dy = float(aY - cam->lastY);

			// Update yaw (phi) and pitch (theta)
			cam->phi += dx * kMouseSensitivity;
			cam->theta += dy * kMouseSensitivity;

			// Clamp pitch to avoid gimbal lock
			if (cam->theta > kPi_ / 2.f)
				cam->theta = kPi_ / 2.f;
			else if (cam->theta < -kPi_ / 2.f)
				cam->theta = -kPi_ / 2.f;
		}

		// Always update last position for smooth delta calculation
		cam->lastX = float(aX);
		cam->lastY = float(aY);
	}

	void glfw_callback_mouse_button_( GLFWwindow* aWindow, int aButton, int aAction, int )
	{
		// Right mouse button toggles camera control
		if (aButton == GLFW_MOUSE_BUTTON_RIGHT && aAction == GLFW_PRESS)
		{
			auto* cam = static_cast<CamCtrl_*>(glfwGetWindowUserPointer(aWindow));
			if (!cam) return;

			cam->cameraActive = !cam->cameraActive;

			if (cam->cameraActive)
			{
				glfwSetInputMode(aWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				std::print("Camera control ACTIVE (WASD/QE to move, mouse to look, right-click to toggle)\n");
			}
			else
			{
				glfwSetInputMode(aWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				std::print("Camera control INACTIVE (right-click to activate)\n");
			}
		}
	}
}

namespace
{
	GLFWCleanupHelper::~GLFWCleanupHelper()
	{
		glfwTerminate();
	}

	GLFWWindowDeleter::~GLFWWindowDeleter()
	{
		if( window )
			glfwDestroyWindow( window );
	}
}
