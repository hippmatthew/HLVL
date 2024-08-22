#ifndef physp_core_stagingbuffer_hpp
#define physp_core_stagingbuffer_hpp

#include "src/core/include/device.hpp"

namespace pp
{

class IResource;

class StagingBuffer
{
  friend class Allocator;

  public:
    StagingBuffer(const Device&);
    StagingBuffer(StagingBuffer&) = delete;
    StagingBuffer(StagingBuffer&&);

    ~StagingBuffer() = default;

    StagingBuffer& operator = (StagingBuffer&) = delete;
    StagingBuffer& operator = (StagingBuffer&&) = delete;

  private:
    unsigned int findIndex(const vk::raii::PhysicalDevice&, unsigned int, vk::MemoryPropertyFlags) const;

    void allocate(const Device&, const std::size_t&, void *);
    void transfer(const Device&, const vk::raii::Buffer&);
    void reset();

  private:
    FamilyType queueFamily = All;

    vk::raii::DeviceMemory vk_memory = nullptr;
    vk::raii::Buffer vk_buffer = nullptr;

    vk::raii::CommandPool vk_commandPool = nullptr;
    vk::raii::CommandBuffer vk_commandBuffer = nullptr;

    vk::raii::Fence vk_fence = nullptr;
};

} // namespace pp

#endif // physp_core_stagingbuffer_hpp