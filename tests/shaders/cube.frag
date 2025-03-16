#version 460

layout(set = 0, binding = 0) uniform sampler2D image;

layout(push_constant) uniform push_constants {
  vec3 color;
};

layout(location = 0) in vec2 uv;

layout(location = 0) out vec4 fragColor;

void main() {
  fragColor = texture(image, uv) * vec4(color, 1.0);
}