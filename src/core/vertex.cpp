#include "src/core/include/vertex.hpp"

namespace hlvl {

Vertex::Vertex(la::vec<3> v) {
  position = v;
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
    }
  };
}

} // namespace hlvl