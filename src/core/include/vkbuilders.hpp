#pragma once

#include "src/core/include/context.hpp"

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan_raii.hpp>

#include <vector>

namespace hlvl {

class BufferBuilder {
  public:
    BufferBuilder() = default;
    BufferBuilder(BufferBuilder&) = delete;
    BufferBuilder(BufferBuilder&&) = delete;

    ~BufferBuilder() = default;

    BufferBuilder& operator = (BufferBuilder&) = delete;
    BufferBuilder& operator = (BufferBuilder&&) = delete;

    BufferBuilder& clear();
    BufferBuilder& new_buffer(
      unsigned int size,
      vk::BufferUsageFlags,
      vk::SharingMode sharing = vk::SharingMode::eExclusive
    );
    void allocate(vk::MemoryPropertyFlags);

    std::vector<unsigned int>&& retrieve_offsets();
    std::vector<vk::raii::Buffer>&& retrieve_buffers();
    vk::raii::DeviceMemory&& retrieve_memory();

  private:
    unsigned int findMemoryIndex(unsigned int filter, vk::MemoryPropertyFlags) const;

  private:
    std::vector<unsigned int> offsets;
    std::vector<vk::raii::Buffer> buffers;
    vk::raii::DeviceMemory memory = nullptr;
};

class CommandBufferBuilder {
  public:
    CommandBufferBuilder(vk::CommandPoolCreateFlags, unsigned int, QueueFamilyType);
    CommandBufferBuilder(CommandBufferBuilder&) = delete;
    CommandBufferBuilder(CommandBufferBuilder&&) = delete;

    ~CommandBufferBuilder() = default;

    CommandBufferBuilder& operator = (CommandBufferBuilder&) = delete;
    CommandBufferBuilder& operator = (CommandBufferBuilder&&) = delete;

    void new_pool(vk::CommandPoolCreateFlags, unsigned int, QueueFamilyType);

    vk::raii::CommandPool&& retrieve_pool();
    vk::raii::CommandBuffers&& retrieve_buffers();

  private:
    vk::raii::CommandPool pool = nullptr;
    vk::raii::CommandBuffers buffers = nullptr;
};

} // namespace hlvl