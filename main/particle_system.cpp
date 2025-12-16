#include "particle_system.hpp"
#include "texture.hpp"
#include <cmath>
#include <print>

ParticleSystem::ParticleSystem(size_t maxParticles)
    : maxParticles(maxParticles), vao(0), vbo(0), texture(0)
{
    particles.resize(maxParticles);
    
    // 初始化所有粒子为死亡状态
    for (auto& p : particles) {
        p.isAlive = false;
        p.life = 0.0f;
    }
}

ParticleSystem::~ParticleSystem() {
    cleanup();
}

void ParticleSystem::initialize(const char* texturePath) {
    // 打印纹理路径
    std::print("Loading particle texture from: {}\n", texturePath);
    
    // 加载纹理
    texture = load_texture_2d(texturePath);
    
    
    // 创建 VAO 和 VBO
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    
    std::print("VAO: {}, VBO: {}\n", vao, vbo);
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    
    // 设置顶点属性指针
  // Layout: position(3) - 纹理坐标由gl_PointCoord自动生成
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
    
    std::print("Particle system initialized with GL_POINTS billboard\n");
}

void ParticleSystem::emit(const Vec3f& position, float lifetime) {
    // 找到一个死亡的粒子槽位
    for (auto& p : particles) {
    if (!p.isAlive) {
            p.position = position;
         p.life = lifetime;
            p.isAlive = true;
    return;
        }
}
    
    // 如果所有粒子都在用，替换第一个（可选策略）
    particles[0].position = position;
    particles[0].life = lifetime;
    particles[0].isAlive = true;
}

void ParticleSystem::update(float dt) {
    for (auto& p : particles) {
  if (!p.isAlive) continue;
     
        // 减少生命值
p.life -= dt;
        
 // 死亡检测
   if (p.life <= 0.0f) {
            p.isAlive = false;
    }
    }
}

void ParticleSystem::render(GLuint shaderProgram, const float* viewMatrix, const float* projMatrix) {
  // 生成顶点数据（只有粒子位置）
    std::vector<float> vertexData;
    vertexData.reserve(particles.size() * 3);
    
    for (const auto& p : particles) {
        if (!p.isAlive) continue;
        
        vertexData.push_back(p.position.x);
        vertexData.push_back(p.position.y);
        vertexData.push_back(p.position.z);
    }
    
    int numParticles = vertexData.size() / 3;
 if (numParticles == 0) return; // 没有存活的粒子
    
    // 上传粒子位置数据到 GPU
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STREAM_DRAW);
    
    // 启用点精灵（Point Sprites）
    glEnable(GL_PROGRAM_POINT_SIZE);  // 允许shader控制点大小
    
    // 启用混合
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // 禁用深度写入（但保留深度测试）
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
    
    // 设置顶点属性（粒子位置）
    glBindVertexArray(vao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // 使用 GL_POINTS 渲染（OpenGL自动生成billboard）
    glDrawArrays(GL_POINTS, 0, numParticles);
    
    glBindVertexArray(0);
    
    // 恢复状态
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glDisable(GL_PROGRAM_POINT_SIZE);
}

void ParticleSystem::cleanup() {
    if (vao) {
        glDeleteVertexArrays(1, &vao);
        vao = 0;
    }
    if (vbo) {
        glDeleteBuffers(1, &vbo);
      vbo = 0;
    }
    if (texture) {
 glDeleteTextures(1, &texture);
        texture = 0;
    }
}
