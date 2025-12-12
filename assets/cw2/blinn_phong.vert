#version 430

// Vertex attributes
layout(location = 0) in vec3 aPosition;
layout(location = 2) in vec3 aNormal;

// Material attributes
layout(location = 3) in float aNs;
layout(location = 4) in vec3 aKa;
layout(location = 5) in vec3 aKd;
layout(location = 6) in vec3 aKs;

// Uniforms
layout(location = 0) uniform mat4 uProjCameraWorld;
layout(location = 1) uniform mat3 uNormalMatrix;

// Outputs to fragment shader
out vec3 vPosition;
out vec3 vNormal;
out float vNs;
out vec3 vKa;
out vec3 vKd;
out vec3 vKs;

void main() {
    // Transform position to clip space
    vec4 worldPos = vec4(aPosition, 1.0);
    gl_Position = uProjCameraWorld * worldPos;
    
    // Pass world position to fragment shader (for specular calculation)
    vPosition = aPosition;
    
    // Transform normal to world space
    vNormal = normalize(uNormalMatrix * aNormal);
  
    // Pass through material properties
    vNs = aNs;
    vKa = aKa;
    vKd = aKd;
    vKs = aKs;
}
