#include "src/core/include/vertex.hpp"

namespace hlvl {

Vertex::Vertex(la::vec<3> pos, la::vec<2> tex) {
  position = pos;
  uv = tex;
}

bool Vertex::operator == (const Vertex& rhs) const {
  return position == rhs.position && uv == rhs.uv;
}

vk::VertexInputBindingDescription Vertex::binding() {
  return vk::VertexInputBindingDescription{
    .binding    = 0,
    .stride     = sizeof(Vertex),
    .inputRate  = vk::VertexInputRate::eVertex
  };
}

std::vector<vk::VertexInputAttributeDescription> Vertex::attributes() {
  return {
    vk::VertexInputAttributeDescription{
      .location = 0,
      .binding  = 0,
      .format   = vk::Format::eR32G32B32Sfloat,
      .offset   = __offsetof(Vertex, position)
    },
    vk::VertexInputAttributeDescription{
      .location = 1,
      .binding  = 0,
      .format   = vk::Format::eR32G32Sfloat,
      .offset   = __offsetof(Vertex, uv)
    }
  };
}

} // namespace hlvl