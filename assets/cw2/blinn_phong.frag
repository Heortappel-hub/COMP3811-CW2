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
    vec3 L = lightPos - fragPos;
    float distance = length(L);
    L = normalize(L);


    float attenuation = 1.0 / (distance * distance);


    float diffuseBoost  = 2.0;
    float specularBoost = 3.0;

    // 漫反射
    float NdotL = max(dot(N, L), 0.0);
    vec3 diffuse = Kd * NdotL * lightColor * attenuation * diffuseBoost;

    // Blinn-Phong
    vec3 H = normalize(L + V);
    float NdotH = max(dot(N, H), 0.0);
    float shininess = max(Ns, 16.0);   
    float spec = pow(NdotH, shininess);

    vec3 specular = Ks * spec * lightColor * attenuation * specularBoost;

    return diffuse + specular;
}


void main() {

    vec3 N = normalize(vNormal);
    vec3 V = normalize(uCameraPos - vPosition);

    // ------------------ 环境光 ------------------
    vec3 ambient = vKa * 0.35;   

    vec3 lighting = ambient;

    // ------------------ 方向光 ------------------
    if (uEnableDirLight == 1) {
        vec3 L = normalize(uLightDir);
        vec3 H = normalize(L + V);

        float NdotL = max(dot(N, L), 0.0);
        vec3 diffuse = vKd * NdotL * 0.6;  

        float NdotH = max(dot(N, H), 0.0);
        float shininess = max(vNs, 16.0);
        float spec = pow(NdotH, shininess);

        vec3 specular = vKs * spec * 1.0; 

        lighting += diffuse + specular;
    }


    if (uEnablePointLight1 == 1) {
        lighting += calculatePointLight(
            uPointLight1Pos,
            uPointLight1Color,
            vPosition,
            N, V,
            vKd, vKs, vNs
        );
    }


    if (uEnablePointLight2 == 1) {
        lighting += calculatePointLight(
            uPointLight2Pos,
            uPointLight2Color,
            vPosition,
            N, V,
            vKd, vKs, vNs
        );
    }


    if (uEnablePointLight3 == 1) {
        lighting += calculatePointLight(
            uPointLight3Pos,
            uPointLight3Color,
            vPosition,
            N, V,
            vKd, vKs, vNs
        );
    }

    // ------------------ 最终输出 ------------------
    fragColor = clamp(lighting, 0.0, 1.0);
}
