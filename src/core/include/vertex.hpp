#pragma once

#include "src/linalg/include/vec.hpp"

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan_beta.h>

#include <vector>

namespace hlvl {

class Vertex {
  public:
    Vertex() = default;
    Vertex(const Vertex&) = default;
    Vertex(Vertex&&) = default;

    ~Vertex() = default;

    Vertex& operator = (const Vertex&) = default;
    Vertex& operator = (Vertex&&) = default;

    static vk::VertexInputBindingDescription binding();
    static std::vector<vk::VertexInputAttributeDescription> attributes();

  public:
    la::vec<3> position;
};

} // namespace hlvl