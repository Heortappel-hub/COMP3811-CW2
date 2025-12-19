#version 430

// 输出
out vec4 FragColor;

// Uniforms
layout(location = 2) uniform sampler2D uTexture;

void main() {
  vec2 texCoord = vec2(gl_PointCoord.x, 1.0 - gl_PointCoord.y);
    
  vec4 texColor = texture(uTexture, texCoord);
    
    // 丢弃完全透明的片段
    if (texColor.a < 0.01) {
        discard;
    }
    
    FragColor = texColor;
}
