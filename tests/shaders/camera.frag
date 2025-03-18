#version 460

layout(set = 0, binding = 0, rgba8) readonly uniform image2D color_data;

layout(push_constant) uniform push_constants {
  uvec2 screen_dims;
  vec3 np_dims;
};

layout(location = 0) in vec2 uv;

layout(location = 0) out vec4 frag_color;

void main() {
  ivec2 coord = ivec2(uv * screen_dims);
  frag_color = imageLoad(color_data, coord);
}