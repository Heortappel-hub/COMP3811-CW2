#version 430
// VAO 提供的属性
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;

// 常量材质属性（通过 glVertexAttrib* 设置）
layout(location = 3) in float aNs;
layout(location = 4) in vec3 aKa;
layout(location = 5) in vec3 aKd;
layout(location = 6) in vec3 aKs;
layout(location = 7) in vec3 aKe;
layout(location = 8) in float aNi;
layout(location = 9) in float aD;
layout(location = 10) in int aIllum;

// Uniform 矩阵
layout(location = 0) uniform mat4 uProjCameraWorld;
layout(location = 1) uniform mat3 uNormalMatrix;

// 传给片段着色器
out vec3 vNormal;
out float vNs;
out vec3 vKa;
out vec3 vKd;

void main() {
    gl_Position = uProjCameraWorld * vec4(aPosition, 1.0);
    vNormal = normalize(uNormalMatrix * aNormal);
    vKa = aKa;
    vKd = aKd;
    vNs = aNs;
}