#include "src/core/include/allocator.hpp"
#include "src/core/include/resource_decl.hpp"
#include "src/core/include/stagingbuffer.hpp"

#include <stdexcept>

namespace pp
{

Allocator::Allocator(std::shared_ptr<Device> device)
: p_device(device), maxThreads(std::thread::hardware_concurrency())
{}

void Allocator::new_allocation(ResourcePool&& pool)
{
  allocations.emplace_back(Allocation(*p_device, std::move(pool), allocations.size(), this));
}

void Allocator::update_allocation(const AllocationIndex& index, void * data, const std::mutex * p_mutex)
{
  validateIndex(index);

  switch(allocations[index.first].locality)
  {
    case host:
      hostUpdate(index, data);
      break;
    case device:
      deviceUpdate(index, data, p_mutex);
  }
}

void Allocator::wait()
{
  while (!futures.empty())
  {
    futures.front().get();
    futures.pop();
  }
}

void Allocator::validateIndex(const AllocationIndex& index)
{
  if (index.first >= allocations.size())
    throw std::out_of_range("pp::Allocator: allocation index out of range");

  if (index.second >= allocations[index.first].buffers.size())
    throw std::out_of_range("pp::Allocator: allocation sub-index out of buffer range");
}

void Allocator::hostUpdate(const AllocationIndex& index, void * data)
{
  auto& allocation = allocations[index.first];
  auto& buffer = allocation.buffers[index.second];

  void * memory = allocation.vk_memory.mapMemory(buffer.offset, buffer.p_resource->size);
  memcpy(memory, data, buffer.p_resource->size);
  allocation.vk_memory.unmapMemory();
}

void Allocator::deviceUpdate(const AllocationIndex& index, void * data, const std::mutex * p_mutex)
{
  if (stagingBuffers.empty())
    stagingBuffers.emplace(StagingBuffer(*p_device));

  auto stagingBuffer = std::move(stagingBuffers.front());
  stagingBuffers.pop();

  auto& allocation = allocations[index.first];
  auto& buffer = allocation.buffers[index.second];

  stagingBuffer.allocate(*p_device, buffer.p_resource->size, data);
  stagingBuffer.transfer(*p_device, buffer.vk_buffer);
  stagingBuffer.reset();

  stagingBuffers.emplace(std::move(stagingBuffer));
}

} // namespace pp