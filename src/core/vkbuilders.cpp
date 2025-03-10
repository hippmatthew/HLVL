#include "src/core/include/settings.hpp"
#include "src/core/include/vkbuilders.hpp"

namespace hlvl {

unsigned int BufferBuilder::allocation_size() const {
  return allocationSize;
}

BufferBuilder& BufferBuilder::clear() {
  offsets.clear();
  buffers.clear();
  memory.clear();
  return *this;
}

BufferBuilder& BufferBuilder::new_buffer(unsigned int size, vk::BufferUsageFlags usage, vk::SharingMode sharing) {
  vk::BufferCreateInfo ci_buffer{
    .size         = size,
    .usage        = usage,
    .sharingMode  = sharing
  };

  buffers.emplace_back(Context::device().createBuffer(ci_buffer));

  return *this;
}

void BufferBuilder::allocate(vk::MemoryPropertyFlags flags) {
  unsigned int filter = ~(0x0);

  for (const auto& buffer : buffers) {
    vk::MemoryRequirements requirements = buffer.getMemoryRequirements();

    unsigned int offset =
      (allocationSize + requirements.alignment - 1) / requirements.alignment * requirements.alignment;
    offsets.emplace_back(offset);

    filter &= requirements.memoryTypeBits;
    allocationSize = offset + requirements.size;
  }

  vk::MemoryAllocateInfo allocateInfo{
    .allocationSize   = allocationSize,
    .memoryTypeIndex  = findMemoryIndex(filter, flags)
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
    if (index == properties.memoryTypeCount) break;

    if ((filter & (1 << index)) && ((memoryType.propertyFlags & flags) == flags))
      return index;
    ++index;
  }

  throw std::runtime_error("hlvl: failed to find a suitable memory index for buffer memory allocation");
}

CommandBufferBuilder::CommandBufferBuilder(vk::CommandPoolCreateFlags flags, unsigned int count, QueueFamilyType type) {
  new_pool(flags, count, type);
}

void CommandBufferBuilder::new_pool(vk::CommandPoolCreateFlags flags, unsigned int count, QueueFamilyType type) {
  pool.clear();
  buffers.clear();

  vk::CommandPoolCreateInfo ci_pool{
    .flags            = flags,
    .queueFamilyIndex = Context::queueIndex(type)
  };

  pool = Context::device().createCommandPool(ci_pool);

  vk::CommandBufferAllocateInfo allocateInfo{
    .commandPool        = pool,
    .level              = vk::CommandBufferLevel::ePrimary,
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

DescriptorSetBuilder::DescriptorSetBuilder(
  const vk::raii::DescriptorSetLayout& vk_dsLayout,
  vk::DescriptorPoolCreateFlags flags,
  unsigned int storages,
  unsigned int uniforms
) {
  new_pool(vk_dsLayout, flags, storages, uniforms);
}

void DescriptorSetBuilder::new_pool(
  const vk::raii::DescriptorSetLayout& vk_dsLayout,
  vk::DescriptorPoolCreateFlags flags,
  unsigned int storages,
  unsigned int uniforms
) {
  pool.clear();
  sets.clear();

  std::vector<vk::DescriptorPoolSize> poolSizes;

  if (storages != 0) {
    poolSizes.emplace_back(vk::DescriptorPoolSize{
      .type             = vk::DescriptorType::eStorageBuffer,
      .descriptorCount  = storages
    });
  }

  if (uniforms != 0) {
    poolSizes.emplace_back(vk::DescriptorPoolSize{
      .type             = vk::DescriptorType::eUniformBuffer,
      .descriptorCount  = uniforms
    });
  }

  vk::DescriptorPoolCreateInfo ci_pool{
    .flags          = flags,
    .maxSets        = hlvl_settings.buffer_mode,
    .poolSizeCount  = static_cast<unsigned int>(poolSizes.size()),
    .pPoolSizes     = poolSizes.data()
  };

  pool = Context::device().createDescriptorPool(ci_pool);

  std::vector<vk::DescriptorSetLayout> layouts;
  for (unsigned int i = 0; i < hlvl_settings.buffer_mode; ++i)
    layouts.emplace_back(vk_dsLayout);

  vk::DescriptorSetAllocateInfo setInfo{
    .descriptorPool     = pool,
    .descriptorSetCount = hlvl_settings.buffer_mode,
    .pSetLayouts        = layouts.data()
  };

  sets = vk::raii::DescriptorSets(Context::device(), setInfo);
}

vk::raii::DescriptorPool&& DescriptorSetBuilder::retrieve_pool() {
  return std::move(pool);
}

vk::raii::DescriptorSets&& DescriptorSetBuilder::retrieve_sets() {
  return std::move(sets);
}

}