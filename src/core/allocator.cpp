#include "src/core/include/allocator.hpp"
#include "src/core/include/resource_decl.hpp"

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

void Allocator::update_allocation(const AllocationIndex& index, void * data)
{
  validateIndex(index);

  switch(allocations[index.first].resourcePool.locality)
  {
    case host:
      hostUpdate(index, data);
      break;
    case device:
      deviceUpdate(index, data);
  }
}

void Allocator::validateIndex(const AllocationIndex& index)
{
  if (index.first >= allocations.size())
    throw std::out_of_range("pp::Allocator: allocation index out of range");

  if (index.second >= allocations[index.first].buffers.size())
    throw std::out_of_range("pp::Allocator: allocation sub-index out of range");
}

void Allocator::hostUpdate(const AllocationIndex& index, void * data)
{
  void * memory = allocations[index.first].vk_memory.mapMemory(
    allocations[index.first].buffers[index.second].offset,
    allocations[index.first].resourcePool.resources[index.second]->size
  );

  memcpy(memory, data, allocations[index.first].resourcePool.resources[index.second]->size);

  allocations[index.first].vk_memory.unmapMemory();
}

void Allocator::deviceUpdate(const AllocationIndex& index, void * data)
{
  if (futures.size() == maxThreads)
  {
    futures.front().get();
    futures.pop();
  }

  std::mutex& dst_mutex = allocations[index.first].buffers[index.second].dst_mutex;
  std::mutex& data_mutex = allocations[index.first].resourcePool.resources[index.second]->mutex();
  const std::size_t& size = allocations[index.first].resourcePool.resources[index.second]->size;
  const vk::raii::Buffer& dst_buffer = allocations[index.first].buffers[index.second].vk_buffer;

  futures.emplace(std::async(std::launch::async,
    [&dst_mutex, &data_mutex, &size, &data, &dst_buffer, this]{
      data_mutex.lock();
      dst_mutex.lock();
      queue_mutex.lock();

      if (stagingBuffers.empty())
        stagingBuffers.emplace(StagingBuffer(*p_device));

      StagingBuffer buffer = std::move(stagingBuffers.front());
      stagingBuffers.pop();

      queue_mutex.unlock();
      buffer.src_mutex.lock();

      buffer.allocate(*p_device, size, data);
      buffer.transfer(*p_device, dst_buffer);

      buffer.src_mutex.unlock();
      data_mutex.unlock();
      dst_mutex.unlock();
      queue_mutex.lock();

      stagingBuffers.emplace(std::move(buffer));

      queue_mutex.unlock();
    }
  ));
}

} // namespace pp