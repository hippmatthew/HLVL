#pragma once

#include "src/core/include/context.hpp"

#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan_beta.h>

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

    unsigned int allocation_size() const;

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
    unsigned int allocationSize = 0;
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

class DescriptorSetBuilder {
  public:
    DescriptorSetBuilder(
      const vk::raii::DescriptorSetLayout&,
      vk::DescriptorPoolCreateFlags,
      unsigned int,
      unsigned int
    );
    DescriptorSetBuilder(DescriptorSetBuilder&) = delete;
    DescriptorSetBuilder(DescriptorSetBuilder&&) = delete;

    ~DescriptorSetBuilder() = default;

    DescriptorSetBuilder& operator = (DescriptorSetBuilder&) = delete;
    DescriptorSetBuilder& operator = (DescriptorSetBuilder&&) = delete;

    void new_pool(
      const vk::raii::DescriptorSetLayout&,
      vk::DescriptorPoolCreateFlags,
      unsigned int,
      unsigned int
    );

    vk::raii::DescriptorPool&& retrieve_pool();
    vk::raii::DescriptorSets&& retrieve_sets();

  private:
    vk::raii::DescriptorPool pool = nullptr;
    vk::raii::DescriptorSets sets = nullptr;
};

} // namespace hlvl