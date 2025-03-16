#version 460

layout(push_constant) uniform push_constants {
  vec3 color;
};

layout(location = 0) in vec2 uv;

layout(location = 0) out vec4 fragColor;

void main() {
  fragColor = vec4(color, 1.0);
}