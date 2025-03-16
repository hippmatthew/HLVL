#pragma once

#include "src/linalg/include/vec.hpp"

#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan_beta.h>

#include <vector>

namespace hlvl {

class Vertex {
  public:
    Vertex() = default;
    Vertex(const Vertex&) = default;
    Vertex(Vertex&&) = default;
    Vertex(la::vec<3>, la::vec<2>);

    ~Vertex() = default;

    Vertex& operator = (const Vertex&) = default;
    Vertex& operator = (Vertex&&) = default;

    bool operator == (const Vertex&) const;

    static vk::VertexInputBindingDescription binding();
    static std::vector<vk::VertexInputAttributeDescription> attributes();

  public:
    la::vec<3> position;
    la::vec<2> uv;
};

} // namespace hlvl