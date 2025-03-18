#version 460

layout(set = 0, binding = 1) uniform sampler2D color_data;

layout(push_constant) uniform push_constants {
  uvec2 screen_dims;
  vec3 np_dims;
};

layout(location = 0) in vec2 uv;

layout(location = 0) out vec4 frag_color;

void main() {
  frag_color = texture(color_data, uv);
}