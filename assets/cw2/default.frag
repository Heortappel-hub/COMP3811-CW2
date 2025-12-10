#version 430

in vec3 vNormal;
in vec2 vTexCoord;   // 新增：接收纹理坐标
in float vNs;
in vec3 vKa;
in vec3 vKd;

layout(location = 2) uniform vec3 uLightDir;
layout(location = 3) uniform sampler2D uTexture;  // 新增：纹理采样器

layout(location = 0) out vec3 fragColor;

void main() {
    vec3 N = normalize(vNormal);
    vec3 L = normalize(uLightDir);

    // 从纹理采样
    vec3 textureColor = texture(uTexture, vTexCoord).rgb;

    // 在着色器中调整材质值
    vec3 ambient = vKa * 0.1;   // 环境光强度：10%
    vec3 diffuse = vKd * 0.7;   // 漫反射强度：70%

    // Lambert 漫反射光照模型
    float NdotL = max(dot(N, L), 0.0);
    
    // 光照计算
    vec3 lighting = ambient + diffuse * NdotL;
    
    // 纹理颜色 × 光照 = 最终颜色
    vec3 color = textureColor * lighting;

    // 确保颜色在有效范围内
    fragColor = clamp(color, 0.0, 1.0);
}