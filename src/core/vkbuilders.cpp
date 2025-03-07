#include "src/core/include/vkbuilders.hpp"

namespace hlvl {

BufferBuilder& BufferBuilder::clear() {
  offsets.clear();
  buffers.clear();
  memory.clear();
  return *this;
}

BufferBuilder& BufferBuilder::new_buffer(unsigned int size, vk::BufferUsageFlags usage, vk::SharingMode sharing) {
  vk::BufferCreateInfo ci_buffer{
    .size = size,
    .usage = usage,
    .sharingMode = sharing
  };

  buffers.emplace_back(Context::device().createBuffer(ci_buffer));

  return *this;
}

void BufferBuilder::allocate(vk::MemoryPropertyFlags flags) {
  vk::DeviceSize allocationSize = 0;
  unsigned int filter = 0x0;

  for (const auto& buffer : buffers) {
    offsets.emplace_back(allocationSize);
    vk::MemoryRequirements requirements = buffer.getMemoryRequirements();

    if (allocationSize % requirements.alignment != 0)
      ++allocationSize;

    filter |= requirements.memoryTypeBits;
    allocationSize += requirements.size;
  }

  vk::MemoryAllocateInfo allocateInfo{
    .allocationSize = allocationSize,
    .memoryTypeIndex = findMemoryIndex(filter, flags)
  };

  memory = Context::device().allocateMemory(allocateInfo);

  for (unsigned int i = 0; i < buffers.size(); ++i)
    buffers[i].bindMemory(memory, offsets[i]);
}

std::vector<unsigned int>&& BufferBuilder::retrieve_offsets() {
  return std::move(offsets);
}

std::vector<vk::raii::Buffer>&& BufferBuilder::retrieve_buffers() {
  return std::move(buffers);
}

vk::raii::DeviceMemory&& BufferBuilder::retrieve_memory() {
  return std::move(memory);
}

unsigned int BufferBuilder::findMemoryIndex(unsigned int filter, vk::MemoryPropertyFlags flags) const {
  vk::PhysicalDeviceMemoryProperties properties = Context::physicalDevice().getMemoryProperties();

  unsigned int index = 0;
  for (const auto& memoryType : properties.memoryTypes) {
    if ((filter & (1 << index)) && (memoryType.propertyFlags & flags) == flags)
      return index;
  }

  throw std::runtime_error("hlvl: failed to find a suitable memory index for buffer memory allocation");
}

CommandBufferBuilder::CommandBufferBuilder(vk::CommandPoolCreateFlags flags, unsigned int count, QueueFamilyType type) {
  new_pool(flags, count, type);
}

void CommandBufferBuilder::new_pool(vk::CommandPoolCreateFlags flags, unsigned int count, QueueFamilyType type) {
  buffers.clear();
  pool.clear();

  vk::CommandPoolCreateInfo ci_pool{
    .flags            = flags,
    .queueFamilyIndex = Context::queueIndex(type)
  };

  pool = Context::device().createCommandPool(ci_pool);

  vk::CommandBufferAllocateInfo allocateInfo{
    .commandPool = pool,
    .level = vk::CommandBufferLevel::ePrimary,
    .commandBufferCount = count
  };

  buffers = vk::raii::CommandBuffers(Context::device(), allocateInfo);
}

vk::raii::CommandPool&& CommandBufferBuilder::retrieve_pool() {
  return std::move(pool);
}

vk::raii::CommandBuffers&& CommandBufferBuilder::retrieve_buffers() {
  return std::move(buffers);
}

}