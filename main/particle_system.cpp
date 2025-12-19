#include "particle_system.hpp"
#include "texture.hpp"
#include <cmath>
#include <print>

ParticleSystem::ParticleSystem(size_t maxParticles): maxParticles(maxParticles), vao(0), vbo(0), texture(0)
{
    particles.resize(maxParticles);
    
    // Pre-allocate vertex buffer to maximum capacity
    vertexData.reserve(maxParticles * 3);
    
	// 初始化所有粒子为死亡状态(initialization)
    for (auto& p : particles) {
        p.isAlive = false;
        p.life = 0.0f;
        p.velocity = Vec3f{0.0f, 0.0f, 0.0f};  // 初始速度为零
    }
}

ParticleSystem::~ParticleSystem() {
    cleanup();
}

void ParticleSystem::initialize(const char* texturePath) {
    
    // 加载纹理
    texture = load_texture_2d(texturePath);
    
    
    // 创建 VAO 和 VBO
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
    
}

void ParticleSystem::emit(const Vec3f& position, float lifetime) {
    // 找到一个死亡的粒子槽位
    for (auto& p : particles) {
     if (!p.isAlive) {
        p.position = position;
            
        p.velocity = Vec3f{0.0f, -2.0f, 0.0f};
         
    p.life = lifetime;
       p.isAlive = true;
    return;
        }
    }
    
    // 如果所有粒子都在用，替换第一个
    particles[0].position = position;
    particles[0].velocity = Vec3f{0.0f, -2.0f, 0.0f};
    particles[0].life = lifetime;
    particles[0].isAlive = true;
}

void ParticleSystem::update(float dt)
{
    for (auto& p : particles)
    {
        if (!p.isAlive)
            continue;

        // 根据速度更新位置
        p.position.x += p.velocity.x * dt;
        p.position.y += p.velocity.y * dt;
        p.position.z += p.velocity.z * dt;

        // 减少生命值
        p.life -= dt;

        // 死亡检测
        if (p.life <= 0.0f)
        {
     p.isAlive = false;
        }
 }
}

void ParticleSystem::render(
    GLuint shaderProgram,
    const float* viewMatrix,
    const float* projMatrix
)
{
    // Clear and reuse vertex buffer (does not deallocate memory)
    vertexData.clear();

    for (const auto& p : particles)
    {
        if (!p.isAlive)
          continue;

        vertexData.push_back(p.position.x);
        vertexData.push_back(p.position.y);
        vertexData.push_back(p.position.z);
    }

    int numParticles = static_cast<int>(vertexData.size() / 3);
    if (numParticles == 0)
        return;

    // 上传粒子位置数据到 GPU
    glBufferData(
        GL_ARRAY_BUFFER,
        maxParticles * 3 * sizeof(float),
        nullptr,
        GL_DYNAMIC_DRAW
    );
    glBufferData(
        GL_ARRAY_BUFFER,
        vertexData.size() * sizeof(float),
        vertexData.data(),
        GL_STREAM_DRAW
    );

	// Active Sprites:This enables the shader to set point size
    glEnable(GL_PROGRAM_POINT_SIZE);

    // 启用混合
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 禁用深度写入
    glDepthMask(GL_FALSE);

    // 使用着色器
    glUseProgram(shaderProgram);

    // 上传矩阵
    glUniformMatrix4fv(0, 1, GL_TRUE, viewMatrix);
    glUniformMatrix4fv(1, 1, GL_TRUE, projMatrix);

    // 绑定纹理
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(2, 0);

    // 设置顶点属性,用粒子位置
    glBindVertexArray(vao);
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(float),
        (void*)0
    );
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_POINTS, 0, numParticles);

    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glDisable(GL_PROGRAM_POINT_SIZE);
}

void ParticleSystem::cleanup()
{
    if (vao)
    {
        glDeleteVertexArrays(1, &vao);
        vao = 0;
    }

    if (vbo)
    {
        glDeleteBuffers(1, &vbo);
        vbo = 0;
    }

    if (texture)
    {
        glDeleteTextures(1, &texture);
        texture = 0;
    }
}