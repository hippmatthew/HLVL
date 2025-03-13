#include "src/core/include/settings.hpp"
#include "src/core/include/vkfactory.hpp"

namespace hlvl {

VulkanFactory::AllocationOutput VulkanFactory::newAllocation(
  const std::vector<vk::BufferCreateInfo>& bufferInfos,
  vk::MemoryPropertyFlags flags
) {
  vk::raii::DeviceMemory memory = nullptr;
  std::vector<vk::raii::Buffer> buffers;
  std::vector<unsigned int> offsets;
  unsigned int allocationSize = 0;
  unsigned int filter = ~(0x0);

  for (const auto& info : bufferInfos)
    buffers.emplace_back(Context::device().createBuffer(info));

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

  return { std::move(memory), std::move(buffers), std::move(offsets), std::move(allocationSize) };
}

VulkanFactory::CommandPoolOutput VulkanFactory::newCommandPool(
  QueueFamilyType queueType,
  unsigned int count,
  vk::CommandPoolCreateFlags flags
) {
  vk::raii::CommandPool pool = nullptr;
  vk::raii::CommandBuffers buffers = nullptr;

  vk::CommandPoolCreateInfo ci_pool{
    .flags            = flags,
    .queueFamilyIndex = Context::queueIndex(queueType)
  };
  pool = Context::device().createCommandPool(ci_pool);

  vk::CommandBufferAllocateInfo allocateInfo{
    .commandPool        = pool,
    .level              = vk::CommandBufferLevel::ePrimary,
    .commandBufferCount = count
  };
  buffers = vk::raii::CommandBuffers(Context::device(), allocateInfo);

  return { std::move(pool), std::move(buffers) };
}

VulkanFactory::DescriptorPoolOutput VulkanFactory::newDescriptorPool(
  const vk::raii::DescriptorSetLayout& vk_dsLayout,
  vk::DescriptorPoolCreateFlags flags,
  unsigned int storages,
  unsigned int uniforms
) {
  vk::raii::DescriptorPool pool = nullptr;
  vk::raii::DescriptorSets sets = nullptr;

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

  return { std::move(pool), std::move(sets) };
}

unsigned int VulkanFactory::findMemoryIndex(unsigned int filter, vk::MemoryPropertyFlags flags) {
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

} // namespace hlvl
