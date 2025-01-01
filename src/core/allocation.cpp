#include "include/allocation.hpp"
#include "include/resource_decl.hpp"

#include <stdexcept>

namespace hlvl
{

Buffer::Buffer(const Device& device, IResource * resource, unsigned long j, bool deviceLocal)
{
  resource->allocation_index.second = j;
  p_resource = resource;

  vk::BufferUsageFlags usage;
  switch (resource->resource_type)
  {
    case vertex:
      usage |= vk::BufferUsageFlagBits::eVertexBuffer;
      break;
    case index:
      usage |= vk::BufferUsageFlagBits::eIndexBuffer;
      break;
    case uniform:
      usage |= vk::BufferUsageFlagBits::eUniformBuffer;
      break;
    case storage:
      usage |= vk::BufferUsageFlagBits::eStorageBuffer;
  }

  if (deviceLocal)
    usage |= vk::BufferUsageFlagBits::eTransferDst;

  vk::BufferCreateInfo ci_buffer{
    .size         = resource->size,
    .usage        = usage,
    .sharingMode  = resource->is_shared ? vk::SharingMode::eConcurrent : vk::SharingMode::eExclusive
  };

  vk_buffer = device.logical().createBuffer(ci_buffer);
}

Buffer::Buffer(Buffer&& buffer)
{
  p_resource = buffer.p_resource;
  vk_buffer = std::move(buffer.vk_buffer);

  buffer.p_resource = nullptr;
}

const vk::raii::Buffer& Buffer::buffer() const
{
  if (*vk_buffer == nullptr)
    throw std::runtime_error("hlvl::Buffer: vk_buffer is null handle");
  return vk_buffer;
}

Allocation::Allocation(const Device& device, ResourcePool&& pool, unsigned long i, Allocator * p_allocator)
{
  unsigned long index = 0;
  unsigned int filter = 0x0u, size = 0;
  locality = pool.locality;

  for (auto * resource : pool.resources)
  {
    resource->allocation_index.first = i;
    resource->p_allocator = p_allocator;

    buffers.emplace_back(Buffer(device, resource, index, locality == Locality::device));
    resource->p_buffer = &buffers[index];

    auto requirements = buffers[index].vk_buffer.getMemoryRequirements();

    while (size % requirements.alignment != 0)
      ++size;

    buffers[index].offset = size;
    filter |= requirements.memoryTypeBits;
    size += requirements.size;

    ++index;
  }

  vk::MemoryPropertyFlags flags;
  switch (locality)
  {
    case Locality::host:
      flags |= vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
      break;
    case Locality::device:
      flags |= vk::MemoryPropertyFlagBits::eDeviceLocal;
  }

  vk::MemoryAllocateInfo allocationInfo{
    .allocationSize = size,
    .memoryTypeIndex  = findIndex(device.physical(), filter, flags)
  };

  vk_memory = device.logical().allocateMemory(allocationInfo);

  for (auto& buffer : buffers)
    buffer.vk_buffer.bindMemory(*vk_memory, buffer.offset);
}

Allocation::Allocation(Allocation&& allocation)
{
  locality = allocation.locality;
  vk_memory = std::move(allocation.vk_memory);
  buffers = std::move(allocation.buffers);
}

unsigned int Allocation::findIndex(
  const vk::raii::PhysicalDevice& vk_physicalDevice,
  unsigned int filter,
  vk::MemoryPropertyFlags flags
) const
{
  unsigned long index = 0;
  for (auto& property : vk_physicalDevice.getMemoryProperties().memoryTypes)
  {
    if ((filter & (1 << index)) && ((property.propertyFlags & flags) == flags))
      return index;

    ++index;
  }

  throw std::runtime_error("hlvl::Allocation: could not find suitable memory type index");
}

} // namespace hlvl