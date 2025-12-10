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

#include "defaults.hpp"

#include "simple_mesh.hpp"
#include "loadobj.hpp"
#include "texture.hpp"  // 新增：纹理加载

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
		
		float phi = 0.f, theta = 0.f;
		float posX = 100.f, posY = 10.f, posZ = 100.f; 
		
		float lastX = 0.f, lastY = 0.f;
	};
	
	void glfw_callback_error_( int, char const* );
	void glfw_callback_key_( GLFWwindow*, int, int, int, int );
	void glfw_callback_motion_( GLFWwindow*, double, double );
	void glfw_callback_mouse_button_( GLFWwindow*, int, int, int );  // 新增

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
	// When building in debug mode, request an OpenGL debug context. This
	// enables additional debugging features. However, this can carry extra
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
	glfwSetMouseButtonCallback( window, &glfw_callback_mouse_button_ );  // 新增鼠标按键回调

	// Set up drawing stuff
	glfwMakeContextCurrent( window );
	glfwSwapInterval( 1 ); // V-Sync is on.

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

	// Load the mesh
	ModelMeshData parlahti_model = load_wavefront_obj_mat( "./assets/cw2/parlahti.obj" );
	ModelMeshData landingpad_model = load_wavefront_obj_mat( "./assets/cw2/landingpad.obj" );
	
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

	// Load texture
	GLuint parlahti_Texture = load_texture_2d("./assets/cw2/L4343A-4k.jpeg");
	std::print("Texture loaded successfully\n");

	// Setup matrices (declared here, updated per-frame)
	Mat44f proj = make_perspective_projection(
		60.f * std::numbers::pi_v<float> / 180.f, 
		float(iwidth) / float(iheight),
		0.1f,
		1000.f  // 增加到1000，之前是200
	);
	Mat44f view;
	Mat44f modelM = kIdentity44f;
	Mat44f uProjCameraWorld;

	// Normal matrix3x3 (identity for now)
	float uNormalMatrix[9] = {
		1.f, 0.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 0.f, 1.f
	};

	// Light direction (world space) - normalized (0, 1, -1)
	// Original: (0, 1, -1), normalized: (0, 0.707107, -0.707107)
	float lightDir[3] = {0.0f, 1.0f, -1.0f};

	// Model transform for the map
	Mat44f map2world = make_translation({ 0.f, 0.f, 0.f });
	
	// Model transform for landingpad (position below camera)
	Mat44f landingpad2world = make_translation({ 100.f, -6.0f, 100.f }) * make_scaling(7.f, 7.f, 7.f);

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
		
		// Update camera position based on input state
		if (camControl.cameraActive)
		{
			// 速度调节：Shift 加速 3 倍，Ctrl 减速到 1/3
			float speedMultiplier = 1.0f;
			if (camControl.shiftPressed)
				speedMultiplier = 3.0f;      // Shift: 3x 速度
			else if (camControl.ctrlPressed)
				speedMultiplier = 1.0f / 3.0f; // Ctrl: 1/3x 速度
			
			float moveSpeed = kMovementSpeed * dt * speedMultiplier;

			// Forward/Back movement (along view direction in XZ plane)
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

		// Draw landingpad
		// Update model-view matrix for landingpad
		Mat44f landingpadModelView = view * landingpad2world;
		Mat44f landingpadNormalMat4 = transpose(landingpadModelView);
		
		// Extract 3x3 part for landingpad normal matrix
		uNormalMatrix[0] = landingpadNormalMat4.v[0]; uNormalMatrix[1] = landingpadNormalMat4.v[1]; uNormalMatrix[2] = landingpadNormalMat4.v[2];
		uNormalMatrix[3] = landingpadNormalMat4.v[4]; uNormalMatrix[4] = landingpadNormalMat4.v[5]; uNormalMatrix[5] = landingpadNormalMat4.v[6];
		uNormalMatrix[6] = landingpadNormalMat4.v[8]; uNormalMatrix[7] = landingpadNormalMat4.v[9]; uNormalMatrix[8] = landingpadNormalMat4.v[10];

		// Upload landingpad uniforms
		glUniformMatrix4fv(0, 1, GL_TRUE, (proj * view * landingpad2world).v);
		glUniformMatrix3fv(1, 1, GL_TRUE, uNormalMatrix);
		// Light direction is already set, no need to set again
		
		// Draw landingpad
		glBindVertexArray(landingpad_vao);
		glDrawArrays(GL_TRIANGLES, 0, landingpad_vertexCount);
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
					cam->ctrlPressed = true;  // 修复：设置 ctrlPressed
				else if (isRelease)
					cam->ctrlPressed = false; // 修复：设置 ctrlPressed
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
