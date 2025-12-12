#version 430

in vec3 vNormal;
in vec3 vColor;

layout(location = 2) uniform vec3 uLightDir;

layout(location = 0) out vec3 fragColor;

void main() {
    vec3 N = normalize(vNormal);
  vec3 L = normalize(uLightDir);
  
    // Simple Lambert shading with vertex color
    float NdotL = max(dot(N, L), 0.0);
    
    vec3 ambient = vColor * 0.3;
    vec3 diffuse = vColor * NdotL * 0.7;
    
    vec3 color = ambient + diffuse;
    
    fragColor = clamp(color, 0.0, 1.0);
}
