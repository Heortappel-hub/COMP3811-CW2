#version 430

// 输入：粒子中心位置
layout(location = 0) in vec3 aPosition;  // 世界空间位置

// 输出
out vec2 vTexCoord;

// Uniforms
layout(location = 0) uniform mat4 uView;
layout(location = 1) uniform mat4 uProjection;

void main() {
    vec4 viewPos = uView * vec4(aPosition, 1.0);
    
    gl_Position = uProjection * viewPos;
    
    // === 透视正确的点大小计算 ===
    
    float worldSize = 0.2;
    float distance = -viewPos.z;
    
    // 3. 从投影矩阵提取FOV相关参数
    // 对于透视投影矩阵，P[1][1] = 1 / tan(fov/2)
    float fovFactor = uProjection[1][1];
    
    // 4. 视口高度（假设为720，可以作为uniform传入）
    float viewportHeight = 720.0;
    
    // 5. 计算屏幕空间的点大小（像素）
    // pointSize = (worldSize * fovFactor * viewportHeight) / (2.0 * distance)
    gl_PointSize = (worldSize * fovFactor * viewportHeight) / (2.0 * distance);
    
    // 限制最小和最大大小，避免太小或太大
    gl_PointSize = clamp(gl_PointSize, 1.0, 200.0);
}
