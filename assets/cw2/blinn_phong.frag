#version 430

// Inputs from vertex shader
in vec3 vPosition;
in vec3 vNormal;
in float vNs;
in vec3 vKa;
in vec3 vKd;
in vec3 vKs;

// Uniforms
layout(location = 2) uniform vec3 uLightDir;
layout(location = 3) uniform vec3 uCameraPos;

// Point lights (3)
layout(location = 4) uniform vec3 uPointLight1Pos;
layout(location = 5) uniform vec3 uPointLight1Color;
layout(location = 6) uniform vec3 uPointLight2Pos;
layout(location = 7) uniform vec3 uPointLight2Color;
layout(location = 8) uniform vec3 uPointLight3Pos;
layout(location = 9) uniform vec3 uPointLight3Color;

// Light enable switches
layout(location = 10) uniform int uEnablePointLight1;
layout(location = 11) uniform int uEnablePointLight2;
layout(location = 12) uniform int uEnablePointLight3;
layout(location = 13) uniform int uEnableDirLight;

// Shader output
layout(location = 0) out vec3 fragColor;


// 点光源计算
vec3 calculatePointLight(
    vec3 lightPos,
    vec3 lightColor,
    vec3 fragPos,
    vec3 N,
    vec3 V,
    vec3 Kd,
    vec3 Ks,
    float Ns
) {
    // Calculate light direction
    vec3 L = normalize(lightPos - fragPos);
    
    // 1/r² distance attenuation
    float r = length(lightPos - fragPos);
    float attenuation = 1.0 / (r * r);
    
    // Blinn-Phong diffuse
    vec3 diffuse = Kd * max(dot(N, L), 0.0) * lightColor * attenuation;
    
    // Blinn-Phong specular
    vec3 H = normalize(L + V);
    vec3 specular = Ks * pow(max(dot(N, H), 0.0), max(Ns, 16.0)) * lightColor * attenuation;
    
    return diffuse + specular;
}


void main() {
    // 归一化法线和视线方向
    vec3 N = normalize(vNormal);
    vec3 V = normalize(uCameraPos - vPosition);
    
    // 环境光
    vec3 lighting = vKa * 0.35;
    
    // 方向光（太阳光）
    if (uEnableDirLight == 1) {
        vec3 L = normalize(uLightDir);
        
        // 漫反射：Kd * max(N·L, 0) * 0.6
        lighting += vKd * max(dot(N, L), 0.0) * 0.6;
        
        // Blinn-Phong 镜面反射：Ks * max(N·H, 0)^Ns
        vec3 H = normalize(L + V);
        lighting += vKs * pow(max(dot(N, H), 0.0), max(vNs, 16.0));
    }
    
    // Light 1（红色）
    if (uEnablePointLight1 == 1) {
        lighting += calculatePointLight(
            uPointLight1Pos,
            uPointLight1Color,
            vPosition,
            N, V,
            vKd, vKs, vNs
        );
    }
    
    // Light 2（绿色）
    if (uEnablePointLight2 == 1) {
        lighting += calculatePointLight(
            uPointLight2Pos,
            uPointLight2Color,
            vPosition,
            N, V,
            vKd, vKs, vNs
        );
    }
  
    // Light 3（蓝色）
    if (uEnablePointLight3 == 1) {
        lighting += calculatePointLight(
            uPointLight3Pos,
            uPointLight3Color,
            vPosition,
            N, V,
            vKd, vKs, vNs
        );
    }
    
    // Final color（限制在 [0, 1] 范围）
    fragColor = clamp(lighting, 0.0, 1.0);
}
