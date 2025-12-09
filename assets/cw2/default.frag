#version 430

in vec3 vNormal;
in float vNs;
in vec3 vKa;
in vec3 vKd;

layout(location = 2) uniform vec3 uLightDir;
layout(location = 0) out vec3 fragColor;

void main() {
    vec3 N = normalize(vNormal);
    vec3 L = normalize(uLightDir);

    // 在着色器中调整材质值 - 更灵活的方式
    vec3 ambient = vKa * 0.1;   // 环境光强度：10%
    vec3 diffuse = vKd * 0.7;   // 漫反射强度：70%

    // Lambert 漫反射光照模型
    float NdotL = max(dot(N, L), 0.0);
    vec3 color = ambient + diffuse * NdotL;

    // 确保颜色在有效范围内
    fragColor = clamp(color, 0.0, 1.0);
}