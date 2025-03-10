#version 460

layout(set = 0, binding = 0) uniform TestUniform {
  vec3 color;
} testUniform;

layout(location = 0) out vec4 fragColor;

void main() {
  fragColor = vec4(testUniform.color, 1.0);
}