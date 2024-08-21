#include "src/core/include/stagingbuffer.hpp"
#include "src/core/include/resource_decl.hpp"
#include "vulkan/vulkan.hpp"
#include <limits>

namespace pp
{

StagingBuffer::StagingBuffer(const Device& device)
{
  if (device.hasFamily(Transfer))
    queueFamily = Transfer;
  else if (device.hasFamily(Async))
    queueFamily = Async;

  vk::CommandPoolCreateInfo ci_commandPool{
    .queueFamilyIndex = static_cast<unsigned int>(device.familyIndex(queueFamily)),
  };

  vk_commandPool = device.logical().createCommandPool(ci_commandPool);

  vk::CommandBufferAllocateInfo commandAllocation{
    .commandPool        = *vk_commandPool,
    .level              = vk::CommandBufferLevel::ePrimary,
    .commandBufferCount = 1
  };

  vk_commandBuffer = std::move(device.logical().allocateCommandBuffers(commandAllocation)[0]);

  vk_fence = device.logical().createFence(vk::FenceCreateInfo{});
}

StagingBuffer::StagingBuffer(StagingBuffer&& buffer)
{
  queueFamily = buffer.queueFamily;

  vk_memory = std::move(buffer.vk_memory);
  vk_buffer = std::move(buffer.vk_buffer);

  vk_commandPool = std::move(buffer.vk_commandPool);
  vk_commandBuffer = std::move(buffer.vk_commandBuffer);
}

unsigned int StagingBuffer::findIndex(
  const vk::raii::PhysicalDevice& vk_physicalDevice,
  unsigned int filter,
  vk::MemoryPropertyFlags flags
) const
{
  unsigned int index = 0;
  for (auto& property : vk_physicalDevice.getMemoryProperties().memoryTypes)
  {
    if ((filter & (1 << index)) && ((property.propertyFlags & flags) == flags))
      return index;
    ++index;
  }

  throw std::runtime_error("pp::StagingBuffer: could not find suitable memory type index");
}

void StagingBuffer::allocate(const Device& device, const std::size_t& size, void * data)
{
  vk::BufferCreateInfo ci_buffer{
    .size         = size,
    .usage        = vk::BufferUsageFlagBits::eTransferSrc,
    .sharingMode  = vk::SharingMode::eExclusive
  };

  vk_buffer = device.logical().createBuffer(ci_buffer);

  auto requirements = vk_buffer.getMemoryRequirements();

  vk::MemoryAllocateInfo allocationInfo{
    .allocationSize = requirements.size,
    .memoryTypeIndex  = findIndex(
      device.physical(),
      requirements.memoryTypeBits,
      vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
    )
  };

  vk_memory = device.logical().allocateMemory(allocationInfo);
  vk_buffer.bindMemory(*vk_memory, 0);

  void * memory = vk_memory.mapMemory(0, size);
  memcpy(memory, data, size);
  vk_memory.unmapMemory();
}

void StagingBuffer::transfer(const Device& device, const vk::raii::Buffer& dst_buffer)
{
  vk_commandBuffer.reset();
  vk_commandBuffer.begin(vk::CommandBufferBeginInfo{});

  vk::BufferCopy copyInfo{
    .srcOffset  = 0,
    .dstOffset  = 0,
    .size       = vk_buffer.getMemoryRequirements().size
  };

  vk_commandBuffer.copyBuffer(*vk_buffer, *dst_buffer, copyInfo);
  vk_commandBuffer.end();

  vk::SubmitInfo submission{
    .commandBufferCount = 1,
    .pCommandBuffers    = &*vk_commandBuffer
  };

  device.queue(queueFamily).submit(submission, *vk_fence);

  static_cast<void>(device.logical().waitForFences(*vk_fence, vk::True, std::numeric_limits<unsigned long>::max()));
  device.logical().resetFences(*vk_fence);
}

} // namespace pp