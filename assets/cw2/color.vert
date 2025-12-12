#version 430

layout(location = 0) in vec3 aPosition;
layout(location = 2) in vec3 aNormal;
layout(location = 5) in vec3 aColor;  // Vertex color

layout(location = 0) uniform mat4 uProjCameraWorld;
layout(location = 1) uniform mat3 uNormalMatrix;

out vec3 vNormal;
out vec3 vColor;

void main() {
    gl_Position = uProjCameraWorld * vec4(aPosition, 1.0);
    vNormal = normalize(uNormalMatrix * aNormal);
    vColor = aColor;
}
