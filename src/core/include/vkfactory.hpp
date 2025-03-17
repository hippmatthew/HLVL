#pragma once

#include "src/core/include/context.hpp"
#include "vulkan/vulkan_enums.hpp"

#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan_beta.h>

#include <tuple>
#include <vector>

namespace hlvl {

class VulkanFactory {
  using AllocationOutput = std::tuple<
    vk::raii::DeviceMemory,
    std::vector<vk::raii::Buffer>,
    std::vector<unsigned int>,
    unsigned int
  >;

  using CommandPoolOutput = std::pair<
    vk::raii::CommandPool,
    vk::raii::CommandBuffers
  >;

  using DescriptorPoolOutput = std::pair<
    vk::raii::DescriptorPool,
    std::vector<vk::raii::DescriptorSets>
  >;

  using TextureOutput = std::tuple<
    vk::raii::DeviceMemory,
    std::vector<vk::raii::Image>,
    std::vector<vk::raii::ImageView>,
    std::vector<vk::raii::Sampler>
  >;

  using DepthOutput = std::tuple<
    vk::raii::DeviceMemory,
    std::vector<vk::raii::Image>,
    std::vector<vk::raii::ImageView>
  >;

  using PNG = std::tuple<
    unsigned char *,
    unsigned int,
    unsigned int,
    unsigned int
  >;

  public:
    VulkanFactory() = delete;
    VulkanFactory(VulkanFactory&) = delete;
    VulkanFactory(VulkanFactory&&) = delete;

    ~VulkanFactory() = default;

    VulkanFactory& operator = (VulkanFactory&) = delete;
    VulkanFactory& operator = (VulkanFactory&&) = delete;

    static AllocationOutput newAllocation(const std::vector<vk::BufferCreateInfo>&, vk::MemoryPropertyFlags);
    static CommandPoolOutput newCommandPool(QueueFamilyType, unsigned int, vk::CommandPoolCreateFlags);
    static DescriptorPoolOutput newDescriptorPool(
      vk::DescriptorPoolCreateFlags,
      const std::vector<vk::raii::DescriptorSetLayout>&,
      unsigned int,
      unsigned int
    );
    static TextureOutput newTextureAllocation(const std::vector<std::string>&);
    static DepthOutput newDepthAllocation(unsigned int);

  private:
    static unsigned int findMemoryIndex(unsigned int, vk::MemoryPropertyFlags);
    static PNG readPNG(const std::string&);
};

} // namespace hlvl