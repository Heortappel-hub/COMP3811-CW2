#pragma once

#include <glad/glad.h>
#include <vector>
#include "../vmlib/vec3.hpp"

struct Particle {
    Vec3f position;  // 世界空间位置
    Vec3f velocity;  // 速度
    float life;      // 剩余生命周期（秒）
    bool isAlive;    // 是否存活
};

// 粒子系统类
class ParticleSystem {
public:
    // Create a particle system with a given maximum number of particles.
    ParticleSystem(size_t maxParticles = 100);
    ~ParticleSystem();
    
    // 初始化
    void initialize(const char* texturePath);
    
    // 发射粒子
    void emit(const Vec3f& position, float lifetime = 1.0f);
    
    // 更新粒子
    void update(float dt);
    
	// 渲染粒子
    void render(GLuint shaderProgram, const float* viewMatrix, const float* projMatrix);
    
	// 清理
    void cleanup();
    
private:
    std::vector<Particle> particles;
    size_t maxParticles;
 
    GLuint vao;
    GLuint vbo;
    GLuint texture;
    
    // Reusable vertex buffer to avoid per-frame allocations
    std::vector<float> vertexData;
};
