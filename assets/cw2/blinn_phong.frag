#version 430

// Inputs from vertex shader
in vec3 vPosition;
in vec3 vNormal;
in float vNs;
in vec3 vKa;
in vec3 vKd;
in vec3 vKs;


layout(location = 2) uniform vec3 uLightDir;
layout(location = 3) uniform vec3 uCameraPos;
layout(location = 0) out vec3 fragColor;

void main() {
    vec3 N = normalize(vNormal);
    vec3 L = normalize(uLightDir);
    vec3 V = normalize(uCameraPos - vPosition);
    
    vec3 H = normalize(L + V);
    
    vec3 ambient = vKa * 0.4;    // ≤ª≥Àª·∫‹¡¡
    
    float NdotL = max(dot(N, L), 0.0);
    vec3 diffuse = vKd * NdotL * 0.6;
    
    // Specular component (Blinn-Phong) - ENHANCED
    float NdotH = max(dot(N, H), 0.0);
    
    // Use higher shininess if vNs is too low
    float shininess = max(vNs, 32.0);  // At least 32
    float specular = pow(NdotH, shininess);
    
    // Boost specular if vKs is too dark
    vec3 specularColor = vKs;
    if (length(vKs) < 0.1) {
        // If material has no specular, add white specular
        specularColor = vec3(0.8, 0.8, 0.8);
    }
    
    vec3 finalSpecular = specularColor * specular * 1.5;  // 1.5x boost
    

    vec3 color = ambient + diffuse + finalSpecular;
    
    // Output final color
    fragColor = clamp(color, 0.0, 1.0);
}
