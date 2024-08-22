#ifndef physp_core_allocator_hpp
#define physp_core_allocator_hpp

#include "src/core/include/allocation.hpp"
#include "src/core/include/stagingbuffer.hpp"

#include <future>
#include <queue>

namespace pp
{

class Allocator
{
  public:
    using AllocationIndex = std::pair<unsigned long, unsigned long>;

  public:
    Allocator(std::shared_ptr<Device>);
    Allocator(Allocator&) = delete;
    Allocator(Allocator&&) = delete;

    ~Allocator() = default;

    Allocator& operator = (Allocator&) = delete;
    Allocator& operator = (Allocator&&) = delete;

    void new_allocation(ResourcePool&&);
    void update_allocation(const AllocationIndex&, void *, const std::mutex *);
    void wait();

  private:
    void validateIndex(const AllocationIndex&);

    void hostUpdate(const AllocationIndex&, void *);
    void deviceUpdate(const AllocationIndex&, void *, const std::mutex *);

  private:
    std::shared_ptr<Device> p_device;

    std::vector<Allocation> allocations;

    const std::size_t maxThreads;
    std::queue<StagingBuffer> stagingBuffers;
    std::queue<std::future<void>> futures;
    std::mutex mutex;
};

} // namespace pp

#endif // physp_core_allocator_hpp