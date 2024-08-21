#ifndef physp_core_allocation_hpp
#define physp_core_allocation_hpp

#include "src/core/include/device.hpp"

namespace pp
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
    vk::raii::Buffer vk_buffer = nullptr;
    unsigned long offset = 0;
    std::mutex dst_mutex;
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
    ResourcePool resourcePool;
    vk::raii::DeviceMemory vk_memory = nullptr;
    std::vector<Buffer> buffers;
};

} // namespace pp

#endif // physp_core_allocation_hpp