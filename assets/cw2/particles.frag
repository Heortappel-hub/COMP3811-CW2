#version 430

// 输出
out vec4 FragColor;

// Uniforms
layout(location = 2) uniform sampler2D uTexture;

void main() {
    // gl_PointCoord 的Y轴是从上到下（左上角=0），需要翻转
    // 纹理坐标的Y轴是从下到上（左下角=0）
  vec2 texCoord = vec2(gl_PointCoord.x, 1.0 - gl_PointCoord.y);
    
  // 采样纹理
    vec4 texColor = texture(uTexture, texCoord);
    
    // 丢弃完全透明的片段
    if (texColor.a < 0.01) {
   discard;
    }
    
    FragColor = texColor;
}
