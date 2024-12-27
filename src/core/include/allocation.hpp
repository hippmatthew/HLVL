#pragma once

#include "device.hpp"

namespace hlvl
{

class Allocator;
class IResource;

enum Locality
{
  host,
  device
};

struct ResourcePool
{
  Locality locality = device;
  std::vector<IResource *> resources;
};

class Buffer
{
  friend class Allocation;
  friend class Allocator;

  public:
    Buffer(const Device&, IResource *, unsigned long, bool);
    Buffer(Buffer&) = delete;
    Buffer(Buffer&&);

    ~Buffer() = default;

    Buffer& operator = (Buffer&) = delete;
    Buffer& operator = (Buffer&&) = delete;

    const vk::raii::Buffer& buffer() const;

  private:
    std::mutex mutex;
    IResource * p_resource = nullptr;

    vk::raii::Buffer vk_buffer = nullptr;
    unsigned long offset = 0;
};

class Allocation
{
  friend class Allocator;

  public:
    Allocation(const Device&, ResourcePool&&, unsigned long, Allocator *);
    Allocation(Allocation&) = delete;
    Allocation(Allocation&&);

    ~Allocation() = default;

    Allocation& operator = (Allocation&) = delete;
    Allocation& operator = (Allocation&&) = delete;

  private:
    unsigned int findIndex(const vk::raii::PhysicalDevice&, unsigned int, vk::MemoryPropertyFlags) const;

  private:
    Locality locality = device;
    vk::raii::DeviceMemory vk_memory = nullptr;
    std::vector<Buffer> buffers;
};

} // namespace hlvl