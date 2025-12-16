#pragma once

#include <glad/glad.h>
#include <vector>
#include "../vmlib/vec3.hpp"

// 粒子结构
struct Particle {
    Vec3f position;  // 世界空间位置
    float life;      // 剩余生命周期（秒）
    bool isAlive;    // 是否存活
};

// 粒子系统类
class ParticleSystem {
public:
    ParticleSystem(size_t maxParticles = 100);
    ~ParticleSystem();
    
    // 初始化（加载纹理和创建VAO）
    void initialize(const char* texturePath);
    
    // 发射粒子
    void emit(const Vec3f& position, float lifetime = 1.0f);
    
    // 更新粒子（减少生命值）
    void update(float dt);
    
    // 渲染粒子
    void render(GLuint shaderProgram, const float* viewMatrix, const float* projMatrix);
    
    // 清理资源
    void cleanup();
    
private:
    std::vector<Particle> particles;
    size_t maxParticles;
 
    GLuint vao;
    GLuint vbo;
    GLuint texture;
    
    // 生成 Billboard 四边形顶点数据
    void generateBillboardData(std::vector<float>& vertexData, const float* viewMatrix);
};
