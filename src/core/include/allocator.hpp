#ifndef physp_core_allocator_hpp
#define physp_core_allocator_hpp

#include "src/core/include/device.hpp"
#include "src/core/include/resource.hpp"

#include <memory>
#include <vector>

namespace pp
{

class AllocationResource
{
  // buffer
  // offset
};

class Allocation
{
  public:

  private:
    // memory
    // allocation resource list
};

class Allocator
{
  public:
    Allocator(std::shared_ptr<Device>);
    Allocator(const Allocator&) = delete;
    Allocator(Allocator&&) = delete;

    ~Allocator() = default;

    Allocator& operator = (const Allocator&) = delete;
    Allocator& operator = (Allocator&&) = delete;

    void create_allocation(const ResourcePool&);
    void create_allocation(const ResourcePool&&);

  private:
    std::shared_ptr<Device> p_device = nullptr;
    std::vector<std::unique_ptr<Allocation>> allocations;
};

}

#endif // physp_core_allocator_hpp