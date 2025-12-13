#version 430

in vec3 vNormal;
in vec2 vTexCoord;
in float vNs;
in vec3 vKa;
in vec3 vKd;

layout(location = 2) uniform vec3 uLightDir;
layout(location = 3) uniform sampler2D uTexture;

layout(location = 0) out vec3 fragColor;

void main() {
  vec3 N = normalize(vNormal);
    vec3 L = normalize(uLightDir);

    // Lambert 漫反射
    float NdotL = max(dot(N, L), 0.0);

    // 纹理采样
    vec3 texColor = texture(uTexture, vTexCoord).rgb;

 // 简单光照：环境光 + 漫反射
    vec3 ambient = texColor * 0.3;        
    vec3 diffuse = texColor * 0.7 * NdotL; 
 
    vec3 color = ambient + diffuse;

    // 确保颜色在有效范围内
    fragColor = clamp(color, 0.0, 1.0);
}