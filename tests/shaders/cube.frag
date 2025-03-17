#version 460

layout(set = 0, binding = 0) uniform sampler2D image;

layout(std140, set = 1, binding = 0) readonly buffer Colors {
  vec3 colors[];
};

layout(push_constant) uniform push_constants {
  vec3 color;
};

layout(location = 0) in vec2 uv;

layout(location = 0) out vec4 fragColor;

void main() {
  uvec2 coord = uvec2(uv * 15);
  uint index = coord.x * 16 + coord.y;

  if (coord.x > 4 && coord.x < 8 && coord.y > 4 && coord.y < 8) {
    fragColor = vec4(color, 1.0);
  }
  else {
    fragColor = texture(image, uv) * vec4(colors[index], 1.0);
  }
}