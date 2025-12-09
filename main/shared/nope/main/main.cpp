#include <glad.h>
#include <GLFW/glfw3.h>

#include <typeinfo>
#include <stdexcept>

#include <cstdio>
#include <cstdlib>
#include <cmath>

#include "../support/error.hpp"
#include "../support/program.hpp"
#include "../support/checkpoint.hpp"
#include "../support/debug_output.hpp"

#include "../vmlib/vec4.hpp"
#include "../vmlib/mat44.hpp"

#include "defaults.hpp"
#include "loadobj.hpp"
#include "test_cube.hpp"
#include "texture.hpp"
#include "../vmlib/mat33.hpp"
#include "buildShapes.hpp"

#include<iostream>

#include "../third_party/glm/glm.hpp"


namespace
{
	constexpr char const* kWindowTitle = "COMP3811 - CW2";

	constexpr float kPi_ = 3.1415926f;

	constexpr float kDefaultSpeed = 5.f;
	constexpr float kMouseSensitivity_ = 0.01f; // radians per pixel

	float movementPerSecond_ = 5.f; // units per second

	bool shiftPressed = 0;
	bool cPressed = 0;

	struct CamCtrl_
	{
		bool cameraActive;

		std::vector<int> cameraModes;
		int currentCameraMode;

		bool forward, back, left, right;

		float phi, theta;
		float forward_back, left_right, up_down;

		float up, down;

		float lastX, lastY;
	};

	struct State_
	{
		ShaderProgram* simple_mesh;
		ShaderProgram* model_mesh;
		ShaderProgram* shape_mesh;

		struct ShipCtrl_ {
			bool FStart;
			bool RReset;
		} shipControl;

		CamCtrl_ camControl_first;
		CamCtrl_ camControl_second;

		std::vector<CamCtrl_*> cameraArray = { &camControl_first , &camControl_second };
	};

	enum CameraModeTable
	{
		FreeMode = 0,
		FollowMode = 1,
		FocusMode = 2
	};

	void glfw_callback_error_(int, char const*);

	void glfw_callback_key_(GLFWwindow*, int, int, int, int);
	void glfw_callback_motion_(GLFWwindow*, double, double);

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





//---------------------------------初始化粒子用的。。。。有人能把这坨放在main外面吗

struct Particle {
	glm::vec3 position; // 位置

	std::vector<Vec2f> texcoords;

	glm::vec3 color;    // 颜色
	float life;         // 生命周期

	Particle() : life(1.0f) {} // 构造函数，初始化生命周期为1.0
};


const int numParticles = 111;
std::vector<Particle> particles;

void initParticles() {
	particles.resize(numParticles);
	for (int i = 0; i < numParticles; ++i) {
		particles[i].position = glm::vec3(

			static_cast<float>(rand() % 400 - 200) / 100.0f,
			static_cast<float>(rand() % 251 - 450) / 100.0f - 0.6f,
			static_cast<float>(rand() % 400 - 200) / 100.0f

		);

		particles[i].texcoords = { { 0.f, 0.f }, { 1.f, 0.f }, { 1.f, 3.f },
			{ 0.f, 0.f }, { 1.f, 3.f }, { 0.f, 3.f } };

		particles[i].color = glm::vec3(
			static_cast<float>(rand() % 100) / 100.0f,
			static_cast<float>(rand() % 100) / 100.0f,
			static_cast<float>(rand() % 100) / 100.0f
		);

		particles[i].life = glm::float32(
			10.0f + static_cast<float>(rand()) / static_cast<float>(RAND_MAX / 10.0f)
		);

	}
}



GLuint createParticleVAO(const std::vector<Particle>& particles) {
	GLuint vao, vbo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	size_t dataSize = particles.size() * sizeof(glm::vec3) * 2; // 位置和颜色
	// 将数据传递到 VBO 中
	glBufferData(GL_ARRAY_BUFFER, dataSize, nullptr, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, dataSize, particles.data());

	// 设置位置属性指针
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, position));
	glEnableVertexAttribArray(0);


	// 设置贴图属性指针
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	return vao;
}



//------------------





int main() try
{
	// Initialize GLFW
	if (GLFW_TRUE != glfwInit())
	{
		char const* msg = nullptr;
		int ecode = glfwGetError(&msg);
		throw Error("glfwInit() failed with '%s' (%d)", msg, ecode);
	}

	// Ensure that we call glfwTerminate() at the end of the program.
	GLFWCleanupHelper cleanupHelper;

	// Configure GLFW and create window
	glfwSetErrorCallback(&glfw_callback_error_);

	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

	//glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_DEPTH_BITS, 24);

#	if !defined(NDEBUG)
	// When building in debug mode, request an OpenGL debug context. This
	// enables additional debugging features. However, this can carry extra
	// overheads. We therefore do not do this for release builds.
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#	endif // ~ !NDEBUG

	GLFWwindow* window = glfwCreateWindow(
		1280,
		720,
		kWindowTitle,
		nullptr, nullptr
	);

	if (!window)
	{
		char const* msg = nullptr;
		int ecode = glfwGetError(&msg);
		throw Error("glfwCreateWindow() failed with '%s' (%d)", msg, ecode);
	}

	GLFWWindowDeleter windowDeleter{ window };

	// Set up event handling
	// TODO: Additional event handling setup

	State_ state{};

	glfwSetWindowUserPointer(window, &state);
	glfwSetKeyCallback(window, &glfw_callback_key_);
	glfwSetCursorPosCallback(window, &glfw_callback_motion_);

	// Set up drawing stuff
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // V-Sync is on.

	// Initialize GLAD
	// This will load the OpenGL API. We mustn't make any OpenGL calls before this!
	if (!gladLoadGLLoader((GLADloadproc)&glfwGetProcAddress))
		throw Error("gladLoaDGLLoader() failed - cannot load GL API!");

	std::printf("RENDERER %s\n", glGetString(GL_RENDERER));
	std::printf("VENDOR %s\n", glGetString(GL_VENDOR));
	std::printf("VERSION %s\n", glGetString(GL_VERSION));
	std::printf("SHADING_LANGUAGE_VERSION %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	// Ddebug output
#	if !defined(NDEBUG)
	setup_gl_debug_output();
#	endif // ~ !NDEBUG

	// Global GL state
	OGL_CHECKPOINT_ALWAYS();

	// TODO: global GL setup goes here

	glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_CULL_FACE);
	glClearColor(0.2f, 0.2f, 0.2f, 0.2f);

	glEnable(GL_DEPTH_TEST);

	OGL_CHECKPOINT_ALWAYS();

	// Get actual framebuffer size.
	// This can be different from the window size, as standard window
	// decorations (title bar, borders, ...) may be included in the window size
	// but not be part of the drawable surface area.
	int iwidth, iheight;
	glfwGetFramebufferSize(window, &iwidth, &iheight);

	glViewport(0, 0, iwidth, iheight);

	// Other initialization & loading
	OGL_CHECKPOINT_ALWAYS();

	// TODO: global GL setup goes here

	ShaderProgram simple_mesh({
		{ GL_VERTEX_SHADER, "assets/default.vert" },
		{ GL_FRAGMENT_SHADER, "assets/default.frag" }
		});

	ShaderProgram model_mesh({
		{ GL_VERTEX_SHADER, "assets/model.vert" },
		{ GL_FRAGMENT_SHADER, "assets/model.frag" }
		});

	ShaderProgram shape_mesh({
		{GL_VERTEX_SHADER, "assets/shape.vert"},
		{GL_FRAGMENT_SHADER, "assets/shape.frag"}
		});


	ShaderProgram point_mesh({
	{GL_VERTEX_SHADER, "assets/point.vert"},
	{GL_FRAGMENT_SHADER, "assets/point.frag"}
		});




	state.simple_mesh = &simple_mesh;
	state.model_mesh = &model_mesh;
	state.shape_mesh = &shape_mesh;

	for (size_t i = 0; i < state.cameraArray.size(); i++)
	{
		state.cameraArray[i]->forward_back = 10.f;
		state.cameraArray[i]->left_right = 0.f;
		state.cameraArray[i]->cameraModes = { FreeMode, FollowMode, FocusMode };
		state.cameraArray[i]->currentCameraMode = 0;
	}

	// Animation state
	auto last = Clock::now();

	float angle = 0.f;

	// Create vertex buffers and VAO
	//TODO: create VBOs and VAO

	// Change to your .obj directory
	SimpleMeshData map = load_wavefront_obj_simplemesh("./assets/parlahti.obj");
	GLuint mapVAO = create_simplemesh_vao(map);
	size_t mapVerticesCount = map.positions.size();

	ModelMeshData landingpad = load_wavefront_obj_modelmesh("./assets/landingpad.obj");
	GLuint landingpadVAO = create_modelmesh_vao(landingpad);
	size_t landingpadVerticesCount = landingpad.positions.size();

	GLuint whiteTexture = load_texture_2d("./assets/white.png");
	GLuint mapTexture = load_texture_2d("./assets/L4343A-4k.jpeg");

	SimpleMeshData cube = make_cube({ 0.663f,0.663f,0.663f });
	GLuint cubeVAO = create_simplemesh_vao(cube);
	size_t cubeVerticesCount = cube.positions.size();

	SimpleMeshData cylinder = make_cylinder(128, { 0.663f,0.663f,0.663f }, make_rotation_z(3.141592f / 2.f));
	GLuint cylinderVAO = create_simplemesh_vao(cylinder);
	size_t cylinderVerticesCount = cylinder.positions.size();

	SimpleMeshData Tetrahedron = make_Tetrahedron({ 0.663f,0.663f,0.663f });
	GLuint TetrahedronVAO = create_simplemesh_vao(Tetrahedron);
	size_t TetrahedronVerticesCount = Tetrahedron.positions.size();



	//---------------初始化点----------------------

	initParticles();
	GLuint test = createParticleVAO(particles);
	std::size_t pointsize = particles.size();

	std::vector<float> pastLifeTimes(particles.size());//生命时间
	for (int i = 0; i < particles.size(); ++i) {
		pastLifeTimes[i] = particles[i].life;
	}
	GLuint floorTexture = load_texture_2d("./assets/explosion.png");
	//---------------------------------------------------





	OGL_CHECKPOINT_ALWAYS();

	int moveCount = 1;
	bool moveStart = 0;
	GLuint64 timePassed = 0;
	GLuint64 mapRenderTime = 0;
	GLuint64 lunchpadRenderTime = 0;
	int frameCount = 0;

	GLuint query[10];  // 使用两个查询对象，轮流使用
	GLuint64 startTime, endTime;
	GLuint64 part2_1, part2_2_1, part2_2_2;// startTime - part2_1, part2_2_1 - part2_2_2
	GLuint64 part4_1_1, part4_1_2, part4_2_1, part4_2_2;// part4_1_1 - part4_1_2, part4_2_1 - part4_2_2
	GLuint64 part5_1_1, part5_1_2;

	// 在初始化中创建查询对象
	glGenQueries(10, query);

	// Main loop
	while (!glfwWindowShouldClose(window))
	{
		// Let GLFW process events
		glfwPollEvents();


		glQueryCounter(query[0], GL_TIMESTAMP);
		glGetQueryObjectui64v(query[0], GL_QUERY_RESULT, &startTime);


		// Check if window was resized.
		int nwidth, nheight;
		float fbwidth, fbheight;
		{
			nwidth, nheight;
			glfwGetFramebufferSize(window, &nwidth, &nheight);

			fbwidth = float(nwidth);
			fbheight = float(nheight);

			if (0 == nwidth || 0 == nheight)
			{
				// Window minimized? Pause until it is unminimized.
				// This is a bit of a hack.
				do
				{
					glfwWaitEvents();
					glfwGetFramebufferSize(window, &nwidth, &nheight);
				} while (0 == nwidth || 0 == nheight);
			}
			//glViewport(0, 0, nwidth, nheight);
		}

		// Update state
		//TODO: update state

		auto const now = Clock::now();
		float dt = std::chrono::duration_cast<Secondsf>(now - last).count();
		last = now;

		angle += dt * kPi_ * 0.1f;
		if (angle >= 2.f * kPi_)
			angle -= 2.f * kPi_;

		// Update camera state
		for (size_t i = 0; i < state.cameraArray.size(); i++)
		{
			if (state.cameraArray[i]->forward)
			{
				state.cameraArray[i]->forward_back -= movementPerSecond_ * dt * cos(state.cameraArray[i]->phi);
				state.cameraArray[i]->left_right += movementPerSecond_ * dt * sin(state.cameraArray[i]->phi);
			}
			else if (state.cameraArray[i]->back)
			{
				state.cameraArray[i]->forward_back += movementPerSecond_ * dt * cos(state.cameraArray[i]->phi);
				state.cameraArray[i]->left_right -= movementPerSecond_ * dt * sin(state.cameraArray[i]->phi);
			}
			else if (state.cameraArray[i]->left)
			{
				state.cameraArray[i]->forward_back -= movementPerSecond_ * dt * sin(state.cameraArray[i]->phi);
				state.cameraArray[i]->left_right -= movementPerSecond_ * dt * cos(state.cameraArray[i]->phi);
			}
			else if (state.cameraArray[i]->right)
			{
				state.cameraArray[i]->forward_back += movementPerSecond_ * dt * sin(state.cameraArray[i]->phi);
				state.cameraArray[i]->left_right += movementPerSecond_ * dt * cos(state.cameraArray[i]->phi);
			}
			else if (state.cameraArray[i]->up)
			{
				state.cameraArray[i]->up_down += movementPerSecond_ * dt;
			}
			else if (state.cameraArray[i]->down)
			{
				state.cameraArray[i]->up_down -= movementPerSecond_ * dt;
			}
		}

		//1.2 part 1

		Mat44f map2world = make_translation({ 0.f, -5.f, 0.f });

		Mat44f Rx = make_rotation_x(0);
		Mat44f Ry = make_rotation_y(0);
		Mat44f T = make_translation({ 0, 0, 0 });

		Mat44f projection = make_perspective_projection(
			60.f * 3.1415926f / 180.f,
			fbwidth / float(fbheight),
			0.1f,
			100.0f);


		glQueryCounter(query[1], GL_TIMESTAMP);
		glGetQueryObjectui64v(query[1], GL_QUERY_RESULT, &part2_1);


		if (state.shipControl.FStart) {
			moveStart = 1;
		}
		else if (state.shipControl.RReset) {
			moveCount = 1;
			moveStart = 0;
		}

		// Update: compute matrices
		//TODO: define and compute projCameraWorld matrix

		std::vector<Vec3f> currentCameraPosArray;
		for (size_t i = 0; i < state.cameraArray.size(); i++)
		{
			currentCameraPosArray.emplace_back(Vec3f{ state.cameraArray[i]->left_right, state.cameraArray[i]->up_down, state.cameraArray[i]->forward_back });
		}

		glQueryCounter(query[5], GL_TIMESTAMP);
		glGetQueryObjectui64v(query[5], GL_QUERY_RESULT, &part4_1_1);

		Mat44f landingpad2world_1 = make_translation({ -10.f, -5.95f, 40.f }) * make_scaling({ 7, 7, 7 });

		Mat44f landingpad2world_2 = make_translation({ 5.f, -5.95f, -10.f }) * make_scaling({ 7, 7, 7 });

		glQueryCounter(query[6], GL_TIMESTAMP);
		glGetQueryObjectui64v(query[6], GL_QUERY_RESULT, &part4_1_2);

		Vec3f shipPos = { 5.f, -8.35f, -10.f };
		Mat44f ship2world = make_translation(shipPos);
		Mat44f ship2worldRotate = make_rotation_y(angle);
		Mat44f ship2worldMove = make_translation({ 0.02f * moveCount, 0.01f * moveCount, 3 * sin(0.01f * moveCount) });
		//Mat44f ship2worldMove = make_translation({ 0, 0.f * moveCount, 3 * sin(0.00f * moveCount) });


		Mat44f tetrahedron2world = make_translation({ 20.f, -5.95f, -5.f });

		if (moveStart) {
			ship2world = ship2world * ship2worldMove * ship2worldRotate;
			if (moveCount < 10000)moveCount++;
		}

		Mat44f cylinder2world = ship2world * make_translation({ 0.f, 3.f, 0.f }) * make_scaling({ 2, 0.6, 2 });

		Mat44f holder2world1 = ship2world * make_translation({ 2.f, 2.8f, 0.f }) * make_scaling({ 0.25, 0.25, 0.25 }) * make_rotation_y(3.141592f / 2.f);
		Mat44f holder2world2 = ship2world * make_translation({ float(sqrt(2)), 2.8f, float(sqrt(2)) }) * make_scaling({ 0.25, 0.25, 0.25 }) * make_rotation_y(3.141592f / 4.f);
		Mat44f holder2world3 = ship2world * make_translation({ 0.f, 2.8f, 2.f }) * make_scaling({ 0.25, 0.25, 0.25 });
		Mat44f holder2world4 = ship2world * make_translation({ -float(sqrt(2)), 2.8f, float(sqrt(2)) }) * make_scaling({ 0.25, 0.25, 0.25 }) * make_rotation_y(-3.141592f / 4.f);
		Mat44f holder2world5 = ship2world * make_translation({ -2.f, 2.8f, 0.f }) * make_scaling({ 0.25, 0.25, 0.25 }) * make_rotation_y(-3.141592f / 2.f);
		Mat44f holder2world6 = ship2world * make_translation({ -float(sqrt(2)), 2.8f, -float(sqrt(2)) }) * make_scaling({ 0.25, 0.25, 0.25 }) * make_rotation_y(3.f * -3.141592f / 4.f);
		Mat44f holder2world7 = ship2world * make_translation({ 0.f, 2.8f, -2.f }) * make_scaling({ 0.25, 0.25, 0.25 }) * make_rotation_y(3.141592f);
		Mat44f holder2world8 = ship2world * make_translation({ float(sqrt(2)), 2.8f, -float(sqrt(2)) }) * make_scaling({ 0.25, 0.25, 0.25 }) * make_rotation_y(3.f * 3.141592f / 4.f);

		Mat44f cube2world = ship2world * make_translation({ 0.f, 3.7f, 0.f }) * make_scaling({ 0.6, 0.6, 0.6 });

		Vec4f currentShipPosition = ship2world * Vec4f({ 0.f, 0.f, 0.f, 1.f });

		Vec3f viewportScale = { 1.f, 0.5f, 1.f };

		Mat44f S = make_scaling(viewportScale);

		std::vector<Mat44f> world2cameraArray;

		for (size_t i = 0; i < state.cameraArray.size(); i++)
		{
			if (state.cameraArray[i]->currentCameraMode == FreeMode)
			{
				Rx = make_rotation_x(state.camControl_first.theta);
				Ry = make_rotation_y(state.camControl_first.phi);
				T = make_translation({ -state.camControl_first.left_right, -state.camControl_first.up_down, -state.camControl_first.forward_back });
			}
			else if (state.cameraArray[i]->currentCameraMode == FollowMode)
			{
				Vec3f offset = { 0, 15, 20 };
				Vec3f objectPosition = { currentShipPosition.x, currentShipPosition.y, currentShipPosition.z };
				//Mat44f objectPosition = make_translation(-ganyuPosition - offset) * make_rotation_y(-angle) * make_scaling({ 1, 1, 1 });
				Rx = make_rotation_x(0.5);
				Ry = make_rotation_y(0);
				T = make_translation(-objectPosition - offset);
			}
			else if (state.cameraArray[i]->currentCameraMode == FocusMode)
			{
				Vec3f offset = { 0, 0, 30 };
				Vec3f objectPosition = { currentShipPosition.x, currentShipPosition.y, currentShipPosition.z };
				Rx = make_rotation_x(atan((objectPosition.y - offset.y) / (objectPosition.z - offset.z)));
				Ry = make_rotation_y(-atan((objectPosition.x - offset.x) / (objectPosition.z - offset.z)));
				T = make_translation(-offset);
			}
			world2cameraArray.emplace_back(S* Rx * Ry * T);
		}



		std::vector<Mat44f> basicCameraWorldArray;
		std::vector<Vec3f> cameraPosArray;
		for (size_t i = 0; i < world2cameraArray.size(); i++)
		{
			basicCameraWorldArray.emplace_back(projection * world2cameraArray[i]);
			cameraPosArray.emplace_back(Vec3f{ state.cameraArray[i]->left_right, state.cameraArray[i]->up_down, state.cameraArray[i]->forward_back });
		}

		Mat33f normalMatrix = mat44_to_mat33(transpose(invert(ship2world)));


		// 1.2 part 2
		glQueryCounter(query[2], GL_TIMESTAMP);
		glGetQueryObjectui64v(query[2], GL_QUERY_RESULT, &part2_2_1);


		// Draw scene
		OGL_CHECKPOINT_DEBUG();

		//TODO: first viewport
		glViewport(0, 0, nwidth / 2, nheight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(simple_mesh.programId());

		// light settings
		glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrix.v);
		Vec3f lightDir1 = normalize(Vec3f{ 0.f, 1.f, -1.f });
		glUniform3fv(2, 1, &lightDir1.x);
		glUniform3f(3, 0.5f, 0.5f, 0.5f);
		glUniform3f(4, 0.6f, 0.6f, 0.6f);

		// draw map
		glBindVertexArray(mapVAO);

		glQueryCounter(query[3], GL_TIMESTAMP);
		glGetQueryObjectui64v(query[3], GL_QUERY_RESULT, &part2_2_2);





		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mapTexture);
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glUniformMatrix4fv(0, 1, GL_TRUE, (basicCameraWorldArray[0] * map2world).v);
		glDrawArrays(GL_TRIANGLES, 0, mapVerticesCount);


		glQueryCounter(query[7], GL_TIMESTAMP);
		glGetQueryObjectui64v(query[7], GL_QUERY_RESULT, &part4_2_1);
		// draw landingpad
		glUseProgram(model_mesh.programId());

		glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrix.v);
		GLfloat lightDir2[3] = { 0.f, 1.f, -1.f };
		GLfloat cameraPosition[3] = { currentCameraPosArray[0].x, currentCameraPosArray[0].y, currentCameraPosArray[0].z };
		glUniform3fv(2, 1, lightDir2);
		glUniform3fv(3, 1, cameraPosition);

		glBindVertexArray(landingpadVAO);

		glUniformMatrix4fv(0, 1, GL_TRUE, (basicCameraWorldArray[0] * landingpad2world_1).v);
		glDrawArrays(GL_TRIANGLES, 0, landingpadVerticesCount);

		glUniformMatrix4fv(0, 1, GL_TRUE, (basicCameraWorldArray[0] * landingpad2world_2).v);
		glDrawArrays(GL_TRIANGLES, 0, landingpadVerticesCount);

		//glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		//glUseProgram(0);
		glQueryCounter(query[8], GL_TIMESTAMP);
		glGetQueryObjectui64v(query[8], GL_QUERY_RESULT, &part4_2_2);



		//-------------------------------粒子效果
		float distance;
		float cz = state.camControl_first.forward_back;
		float cx = state.camControl_first.left_right;
		float cy = state.camControl_first.up_down;

		float nowpositionz = currentShipPosition.z;
		float nowpositionx = currentShipPosition.x;
		float nowpositiony = currentShipPosition.y;

		distance = sqrt(
			pow(nowpositionx - cx, 2) +
			pow(nowpositiony - cy, 2) +
			pow(nowpositionz - cz, 2)
		);

		float particleSize = 1500 / (distance * 10);

		if (moveStart) {
			glBindTexture(GL_TEXTURE_2D, floorTexture);

			glUseProgram(point_mesh.programId());

			glUniform1i(glGetUniformLocation(point_mesh.programId(), "circleTexture"), 0);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


			glPointSize(particleSize);

			glBindVertexArray(test);

			glUniformMatrix4fv(0, 1, GL_TRUE, (basicCameraWorldArray[0] * cube2world).v);

			glActiveTexture(GL_TEXTURE0);

			for (int i = 0; i < particles.size(); ++i) {
				Vec4f particlePos;
				particlePos.x = particles[i].position.x;
				particlePos.y = particles[i].position.y;
				particlePos.z = particles[i].position.z;
				particlePos.w = 1.0f; // 设置 w 分量
				Vec4f transformedPos = cube2world * particlePos;
				particles[i].life = particles[i].life - 0.1;

				if (particles[i].life <= -5) {
					particles[i].life = pastLifeTimes[i];
				}

				if (particles[i].life < 0) {
					continue;
				}
				glDrawArrays(GL_POINTS, i, 1);
			}
		}


		//-------------------------------


		// draw cube 
		glUseProgram(shape_mesh.programId());
		glBindVertexArray(cubeVAO);

		//------------------------------------------------------------//
		glUniform3fv(2, 1, &lightDir1.x);
		glUniform3f(3, 0.5f, 0.1f, 1.0f);//反射光色1
		glUniform3f(4, 0.1f, 0.1f, 0.2f);//环境光

		Vec3f lightPos2 = -lightDir1;  // 光源2的位置
		glUniform3fv(5, 1, &lightPos2.x);//光源2
		glUniform3fv(6, 1, &cameraPosArray[0].x);//相机位置
		float specularIntensity = 0.5f;
		float lightSpecular[3] = { 0.5f, 0.2f, 1.0f }; // 这是一个白色光的例子
		glUniform3fv(7, 1, lightSpecular);
		glUniformMatrix4fv(8, 1, GL_TRUE, (ship2world).v); //绑定变化矩阵 
		glUniform3f(9, 1.0f, 0.0f, 0.0f);//反射光色2

		Vec3f lightPos3 = normalize(Vec3f{ 1.f, 1.5f, -0.5f });
		glUniform3fv(10, 1, &lightPos3.x);//光源3
		glUniform3f(11, 0.0f, 1.0f, 0.0f);//反射光色3

		//-----------------------------------------------------------//


		glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrix.v);
		glUniformMatrix4fv(0, 1, GL_TRUE, (basicCameraWorldArray[0] * cube2world).v);
		glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);
		glBindVertexArray(0);
		glUseProgram(0);


		glUseProgram(shape_mesh.programId());
		glBindVertexArray(cylinderVAO);

		glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrix.v);


		glUniformMatrix4fv(0, 1, GL_TRUE, (basicCameraWorldArray[0] * cylinder2world).v);
		glDrawArrays(GL_TRIANGLES, 0, cylinderVerticesCount);


		glBindVertexArray(0);
		glUseProgram(0);



		//以下是支架部分
		glUseProgram(shape_mesh.programId());
		glBindVertexArray(TetrahedronVAO);


		//------------------------------------------------------------//
		glUniform3fv(6, 1, &cameraPosArray[0].x);//相机位置
		glUniformMatrix4fv(8, 1, GL_TRUE, (holder2world1).v);
		//-----------------------------------------------------------//

		glUniformMatrix4fv(0, 1, GL_TRUE, (basicCameraWorldArray[0] * holder2world1).v);
		Mat33f normalMatrixHolder1 = mat44_to_mat33(transpose(invert(holder2world1)));
		glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrixHolder1.v);
		glDrawArrays(GL_TRIANGLES, 0, TetrahedronVerticesCount);

		glUniformMatrix4fv(0, 1, GL_TRUE, (basicCameraWorldArray[0] * holder2world2).v);
		Mat33f normalMatrixHolder2 = mat44_to_mat33(transpose(invert(holder2world2)));
		glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrixHolder2.v);
		glDrawArrays(GL_TRIANGLES, 0, TetrahedronVerticesCount);

		glUniformMatrix4fv(0, 1, GL_TRUE, (basicCameraWorldArray[0] * holder2world3).v);
		Mat33f normalMatrixHolder3 = mat44_to_mat33(transpose(invert(holder2world3)));
		glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrixHolder3.v);
		glDrawArrays(GL_TRIANGLES, 0, TetrahedronVerticesCount);

		glUniformMatrix4fv(0, 1, GL_TRUE, (basicCameraWorldArray[0] * holder2world4).v);
		Mat33f normalMatrixHolder4 = mat44_to_mat33(transpose(invert(holder2world4)));
		glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrixHolder4.v);
		glDrawArrays(GL_TRIANGLES, 0, TetrahedronVerticesCount);

		glUniformMatrix4fv(0, 1, GL_TRUE, (basicCameraWorldArray[0] * holder2world5).v);
		Mat33f normalMatrixHolder5 = mat44_to_mat33(transpose(invert(holder2world5)));
		glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrixHolder5.v);
		glDrawArrays(GL_TRIANGLES, 0, TetrahedronVerticesCount);

		glUniformMatrix4fv(0, 1, GL_TRUE, (basicCameraWorldArray[0] * holder2world6).v);
		Mat33f normalMatrixHolder6 = mat44_to_mat33(transpose(invert(holder2world6)));
		glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrixHolder6.v);
		glDrawArrays(GL_TRIANGLES, 0, TetrahedronVerticesCount);

		glUniformMatrix4fv(0, 1, GL_TRUE, (basicCameraWorldArray[0] * holder2world7).v);
		Mat33f normalMatrixHolder7 = mat44_to_mat33(transpose(invert(holder2world7)));
		glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrixHolder7.v);
		glDrawArrays(GL_TRIANGLES, 0, TetrahedronVerticesCount);

		glUniformMatrix4fv(0, 1, GL_TRUE, (basicCameraWorldArray[0] * holder2world8).v);
		Mat33f normalMatrixHolder8 = mat44_to_mat33(transpose(invert(holder2world8)));
		glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrixHolder8.v);
		glDrawArrays(GL_TRIANGLES, 0, TetrahedronVerticesCount);








		// TODO: second viewport
		//------------------------------------------------------------------second viewport--------------------------------------------------------------
		glViewport(nwidth - nwidth / 2, 0, nwidth / 2, nheight);

		glUseProgram(simple_mesh.programId());

		// light settings
		glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrix.v);
		lightDir1 = normalize(Vec3f{ 0.f, 1.f, -1.f });
		glUniform3fv(2, 1, &lightDir1.x);
		glUniform3f(3, 0.5f, 0.5f, 0.5f);
		glUniform3f(4, 0.6f, 0.6f, 0.6f);

		// draw map
		glBindVertexArray(mapVAO);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mapTexture);
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glUniformMatrix4fv(0, 1, GL_TRUE, (basicCameraWorldArray[1] * map2world).v);
		glDrawArrays(GL_TRIANGLES, 0, mapVerticesCount);


		// draw landingpad
		glUseProgram(model_mesh.programId());

		glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrix.v);
		GLfloat lightDir3[3] = { 0.f, 1.f, -1.f };
		GLfloat cameraPositionSecondViewport[3] = { currentCameraPosArray[1].x, currentCameraPosArray[1].y, currentCameraPosArray[1].z };
		glUniform3fv(2, 1, lightDir3);
		glUniform3fv(3, 1, cameraPositionSecondViewport);

		glBindVertexArray(landingpadVAO);

		glUniformMatrix4fv(0, 1, GL_TRUE, (basicCameraWorldArray[1] * landingpad2world_1).v);
		glDrawArrays(GL_TRIANGLES, 0, landingpadVerticesCount);

		glUniformMatrix4fv(0, 1, GL_TRUE, (basicCameraWorldArray[1] * landingpad2world_2).v);
		glDrawArrays(GL_TRIANGLES, 0, landingpadVerticesCount);

		//glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		//glUseProgram(0);


		//-------------------------------粒子效果

		if (moveStart) {
			glBindTexture(GL_TEXTURE_2D, floorTexture);

			glUseProgram(point_mesh.programId());

			glUniform1i(glGetUniformLocation(point_mesh.programId(), "circleTexture"), 0);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);



			glPointSize(particleSize);

			glBindVertexArray(test);

			glUniformMatrix4fv(0, 1, GL_TRUE, (basicCameraWorldArray[1] * cube2world).v);

			glActiveTexture(GL_TEXTURE0);

			for (int i = 0; i < particles.size(); ++i) {
				Vec4f particlePos;
				particlePos.x = particles[i].position.x;
				particlePos.y = particles[i].position.y;
				particlePos.z = particles[i].position.z;
				particlePos.w = 1.0f; // 设置 w 分量
				Vec4f transformedPos = cube2world * particlePos;
				particles[i].life = particles[i].life - 0.1;

				if (particles[i].life <= -5) {
					particles[i].life = pastLifeTimes[i];
				}

				if (particles[i].life < 0) {
					continue;
				}
				glDrawArrays(GL_POINTS, i, 1);
			}
		}


		//-------------------------------






		// draw cube 
		glUseProgram(shape_mesh.programId());
		glBindVertexArray(cubeVAO);



		//------------------------------------------------------------//
		glUniform3fv(2, 1, &lightDir1.x);
		glUniform3f(3, 0.5f, 0.1f, 1.0f);//反射光色1
		glUniform3f(4, 0.1f, 0.1f, 0.2f);//环境光

		lightPos2 = -lightDir1;  // 光源2的位置
		glUniform3fv(5, 1, &lightPos2.x);//光源2
		glUniform3fv(6, 1, &cameraPosArray[0].x);//相机位置
		specularIntensity = 0.5f;
		glUniform3fv(7, 1, lightSpecular);
		glUniformMatrix4fv(8, 1, GL_TRUE, (ship2world).v); //绑定变化矩阵 
		glUniform3f(9, 1.0f, 0.0f, 0.0f);//反射光色2

		lightPos3 = normalize(Vec3f{ 1.f, 1.5f, -0.5f });
		glUniform3fv(10, 1, &lightPos3.x);//光源3
		glUniform3f(11, 0.0f, 1.0f, 0.0f);//反射光色3


		//-----------------------------------------------------------//


		glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrix.v);


		glUniformMatrix4fv(0, 1, GL_TRUE, (basicCameraWorldArray[1] * cube2world).v);
		glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

		glBindVertexArray(0);
		glUseProgram(0);



		glUseProgram(shape_mesh.programId());
		glBindVertexArray(cylinderVAO);

		glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrix.v);


		glUniformMatrix4fv(0, 1, GL_TRUE, (basicCameraWorldArray[1] * cylinder2world).v);
		glDrawArrays(GL_TRIANGLES, 0, cylinderVerticesCount);


		glBindVertexArray(0);
		glUseProgram(0);



		//以下是支架部分
		glUseProgram(shape_mesh.programId());
		glBindVertexArray(TetrahedronVAO);


		//------------------------------------------------------------//
		glUniform3fv(6, 1, &cameraPosArray[1].x);//相机位置
		glUniformMatrix4fv(8, 1, GL_TRUE, (holder2world1).v);
		//-----------------------------------------------------------//

		glUniformMatrix4fv(0, 1, GL_TRUE, (basicCameraWorldArray[1] * holder2world1).v);
		normalMatrixHolder1 = mat44_to_mat33(transpose(invert(holder2world1)));
		glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrixHolder1.v);
		glDrawArrays(GL_TRIANGLES, 0, TetrahedronVerticesCount);

		glUniformMatrix4fv(0, 1, GL_TRUE, (basicCameraWorldArray[1] * holder2world2).v);
		normalMatrixHolder2 = mat44_to_mat33(transpose(invert(holder2world2)));
		glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrixHolder2.v);
		glDrawArrays(GL_TRIANGLES, 0, TetrahedronVerticesCount);

		glUniformMatrix4fv(0, 1, GL_TRUE, (basicCameraWorldArray[1] * holder2world3).v);
		normalMatrixHolder3 = mat44_to_mat33(transpose(invert(holder2world3)));
		glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrixHolder3.v);
		glDrawArrays(GL_TRIANGLES, 0, TetrahedronVerticesCount);

		glUniformMatrix4fv(0, 1, GL_TRUE, (basicCameraWorldArray[1] * holder2world4).v);
		normalMatrixHolder4 = mat44_to_mat33(transpose(invert(holder2world4)));
		glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrixHolder4.v);
		glDrawArrays(GL_TRIANGLES, 0, TetrahedronVerticesCount);

		glUniformMatrix4fv(0, 1, GL_TRUE, (basicCameraWorldArray[1] * holder2world5).v);
		normalMatrixHolder5 = mat44_to_mat33(transpose(invert(holder2world5)));
		glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrixHolder5.v);
		glDrawArrays(GL_TRIANGLES, 0, TetrahedronVerticesCount);

		glUniformMatrix4fv(0, 1, GL_TRUE, (basicCameraWorldArray[1] * holder2world6).v);
		normalMatrixHolder6 = mat44_to_mat33(transpose(invert(holder2world6)));
		glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrixHolder6.v);
		glDrawArrays(GL_TRIANGLES, 0, TetrahedronVerticesCount);

		glUniformMatrix4fv(0, 1, GL_TRUE, (basicCameraWorldArray[1] * holder2world7).v);
		normalMatrixHolder7 = mat44_to_mat33(transpose(invert(holder2world7)));
		glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrixHolder7.v);
		glDrawArrays(GL_TRIANGLES, 0, TetrahedronVerticesCount);

		glUniformMatrix4fv(0, 1, GL_TRUE, (basicCameraWorldArray[1] * holder2world8).v);
		normalMatrixHolder8 = mat44_to_mat33(transpose(invert(holder2world8)));
		glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrixHolder8.v);
		glDrawArrays(GL_TRIANGLES, 0, TetrahedronVerticesCount);


		OGL_CHECKPOINT_DEBUG();


		// Display results
		glfwSwapBuffers(window);


		glQueryCounter(query[4], GL_TIMESTAMP);
		glGetQueryObjectui64v(query[4], GL_QUERY_RESULT, &endTime);


		mapRenderTime += (part2_1 - startTime) + (part2_2_2 - part2_2_1);
		lunchpadRenderTime += (part4_1_2 - part4_1_1) + (part4_2_2 - part4_2_1);
		timePassed += endTime - startTime;
		frameCount++;

		if ((timePassed / 1000000000) >= 1) {
			std::cout << "Map Render Time(1.2) = " << mapRenderTime << std::endl;
			std::cout << "Lunchpad Render Time(1.4) = " << lunchpadRenderTime << std::endl;
			std::cout << "FPS = " << frameCount << std::endl;
			timePassed -= 1000000000;
			mapRenderTime = 0;
			frameCount = 0;
		}




	}

	// Cleanup.
	//TODO: additional cleanup

	state.simple_mesh = nullptr;
	state.model_mesh = nullptr;
	state.shape_mesh = nullptr;

	return 0;
}
catch (std::exception const& eErr)
{
	std::fprintf(stderr, "Top-level Exception (%s):\n", typeid(eErr).name());
	std::fprintf(stderr, "%s\n", eErr.what());
	std::fprintf(stderr, "Bye.\n");
	return 1;
}


namespace
{
	void glfw_callback_error_(int aErrNum, char const* aErrDesc)
	{
		std::fprintf(stderr, "GLFW error: %s (%d)\n", aErrDesc, aErrNum);
	}

	void glfw_callback_key_(GLFWwindow* aWindow, int aKey, int, int aAction, int)
	{
		if (GLFW_KEY_ESCAPE == aKey && GLFW_PRESS == aAction)
		{
			glfwSetWindowShouldClose(aWindow, GLFW_TRUE);
			return;
		}

		if (auto* state = static_cast<State_*>(glfwGetWindowUserPointer(aWindow)))
		{
			// R-key reloads shaders.
			if (GLFW_KEY_R == aKey && GLFW_PRESS == aAction)
			{
				if (state->simple_mesh)
				{
					try
					{
						state->simple_mesh->reload();
						std::fprintf(stderr, "Shaders reloaded and recompiled.\n");
					}
					catch (std::exception const& eErr)
					{
						std::fprintf(stderr, "Error when reloading shader:\n");
						std::fprintf(stderr, "%s\n", eErr.what());
						std::fprintf(stderr, "Keeping old shader.\n");
					}
				}
			}

			// Space toggles camera
			if (GLFW_KEY_SPACE == aKey && GLFW_PRESS == aAction)
			{
				state->camControl_first.cameraActive = !state->camControl_first.cameraActive;

				if (state->camControl_first.cameraActive)
					glfwSetInputMode(aWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
				else
					glfwSetInputMode(aWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}

			// Camera controls if camera is active
			if (state->camControl_first.cameraActive)
			{
				if (GLFW_KEY_W == aKey)
				{
					if (GLFW_PRESS == aAction)
						state->camControl_first.forward = true;
					else if (GLFW_RELEASE == aAction)
						state->camControl_first.forward = false;
				}
				else if (GLFW_KEY_S == aKey)
				{
					if (GLFW_PRESS == aAction)
						state->camControl_first.back = true;
					else if (GLFW_RELEASE == aAction)
						state->camControl_first.back = false;
				}
				else if (GLFW_KEY_A == aKey)
				{
					if (GLFW_PRESS == aAction)
						state->camControl_first.left = true;
					else if (GLFW_RELEASE == aAction)
						state->camControl_first.left = false;
				}
				else if (GLFW_KEY_D == aKey)
				{
					if (GLFW_PRESS == aAction)
						state->camControl_first.right = true;
					else if (GLFW_RELEASE == aAction)
						state->camControl_first.right = false;
				}
				else if (GLFW_KEY_Q == aKey)
				{
					if (GLFW_PRESS == aAction)
						state->camControl_first.down = true;
					else if (GLFW_RELEASE == aAction)
						state->camControl_first.down = false;
				}
				else if (GLFW_KEY_E == aKey)
				{
					if (GLFW_PRESS == aAction)
						state->camControl_first.up = true;
					else if (GLFW_RELEASE == aAction)
						state->camControl_first.up = false;
				}
				else if (GLFW_KEY_LEFT_SHIFT == aKey)
				{
					if (GLFW_PRESS == aAction)
					{
						movementPerSecond_ = kDefaultSpeed + 5;
						shiftPressed = 1;
					}
					else if (GLFW_RELEASE == aAction)
					{
						movementPerSecond_ = kDefaultSpeed;
						shiftPressed = 0;
					}
				}
				else if (GLFW_KEY_LEFT_CONTROL == aKey)
				{
					if (GLFW_PRESS == aAction)
						movementPerSecond_ = kDefaultSpeed - 3;
					else if (GLFW_RELEASE == aAction)
						movementPerSecond_ = kDefaultSpeed;
				}
				else if (GLFW_KEY_C == aKey)
				{
					if (GLFW_PRESS == aAction)
					{
						cPressed = 1;
					}
					else if (GLFW_RELEASE == aAction)
					{
						cPressed = 0;
					}
				}
			}

			if (shiftPressed && cPressed)
			{
				state->cameraArray[1]->currentCameraMode = (state->cameraArray[1]->currentCameraMode + 1) % state->cameraArray[1]->cameraModes.size();
			}
			else if (!shiftPressed && cPressed)
			{
				state->cameraArray[0]->currentCameraMode = (state->cameraArray[0]->currentCameraMode + 1) % state->cameraArray[0]->cameraModes.size();
			}

			if (GLFW_KEY_R == aKey) {
				if (GLFW_PRESS == aAction)
					state->shipControl.RReset = 1;
				else if (GLFW_RELEASE == aAction)
					state->shipControl.RReset = 0;
			}

			if (GLFW_KEY_F == aKey) {
				if (GLFW_PRESS == aAction)
					state->shipControl.FStart = 1;
				else if (GLFW_RELEASE == aAction)
					state->shipControl.FStart = 0;
			}
		}
	}

	void glfw_callback_motion_(GLFWwindow* aWindow, double aX, double aY)
	{
		if (auto* state = static_cast<State_*>(glfwGetWindowUserPointer(aWindow)))
		{
			if (state->camControl_first.cameraActive)
			{
				auto const dx = float(aX - state->camControl_first.lastX);
				auto const dy = float(aY - state->camControl_first.lastY);

				state->camControl_first.phi += dx * kMouseSensitivity_;

				state->camControl_first.theta += dy * kMouseSensitivity_;
				if (state->camControl_first.theta > kPi_ / 2.f)
					state->camControl_first.theta = kPi_ / 2.f;
				else if (state->camControl_first.theta < -kPi_ / 2.f)
					state->camControl_first.theta = -kPi_ / 2.f;
			}

			state->camControl_first.lastX = float(aX);
			state->camControl_first.lastY = float(aY);
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
		if (window)
			glfwDestroyWindow(window);
	}
}

