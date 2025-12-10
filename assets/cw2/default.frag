#version 430

in vec3 vNormal;
in vec2 vTexCoord;   // 新增：接收纹理坐标
in float vNs;
in vec3 vKa;
in vec3 vKd;

layout(location = 2) uniform vec3 uLightDir;
layout(location = 3) uniform sampler2D uTexture;
layout(location = 4) uniform int uUseTexture;  // 新增：0 = 使用材质颜色，1 = 使用纹理

layout(location = 0) out vec3 fragColor;

void main() {
    vec3 N = normalize(vNormal);
    vec3 L = normalize(uLightDir);

    // Lambert 漫反射
    float NdotL = max(dot(N, L), 0.0);

    vec3 color;
    
    if (uUseTexture == 1) {
        // 使用纹理模式
        vec3 texColor = texture(uTexture, vTexCoord).rgb;
    
        // 简单光照：环境光 + 漫反射
        vec3 ambient = texColor * 0.3;         // 30% 环境光
        vec3 diffuse = texColor * 0.7 * NdotL; // 70% 漫反射
        
        color = ambient + diffuse;
} else {
// 使用材质颜色模式
        vec3 ambient = vKa * 0.1;
     vec3 diffuse = vKd * 0.7 * NdotL;
        
   color = ambient + diffuse;
    }

    // 确保颜色在有效范围内
    fragColor = clamp(color, 0.0, 1.0);
}