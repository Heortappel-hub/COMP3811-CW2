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
    
    
    float worldSize = 0.2;
    float distance = -viewPos.z;
    
    float fovFactor = uProjection[1][1];
    
    float viewportHeight = 720.0;
    
    gl_PointSize = (worldSize * fovFactor * viewportHeight) / (2.0 * distance);  // billboard size calculation
    
    // 限制最小和最大大小
    gl_PointSize = clamp(gl_PointSize, 1.0, 200.0);
}
