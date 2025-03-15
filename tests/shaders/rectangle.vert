#version 460

layout(set = 1, binding = 0) uniform Matrices {
  layout(row_major) mat4 model;
  layout(row_major) mat4 view;
  layout(row_major) mat4 proj;
};

layout(push_constant) uniform push_constants {
  vec3 color;
};

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uvIn;

layout(location = 0) out vec2 uvOut;

void main() {
  gl_Position = proj * view * model * vec4(position, 1.0);
  uvOut = uvIn;
}