#pragma once

#include "src/core/include/objects.hpp"

#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan_beta.h>

namespace hlvl {

class Renderer {
  friend class Context;

  public:
    Renderer() = default;
    Renderer(Renderer&) = delete;
    Renderer(Renderer&&) = delete;

    ~Renderer() = default;

    Renderer& operator = (Renderer&) = delete;
    Renderer& operator = (Renderer&&) = delete;

  private:
    void checkFormat() const;
    void checkPresentMode() const;
    std::pair<unsigned int, vk::SurfaceTransformFlagBitsKHR> checkExtent() const;

    void init();
    void createImageViews();
    void createSyncObjects();
    void render();
    void beginRendering(unsigned int);
    void renderObject(const Object&);
    void endRendering(unsigned int);

  private:
    unsigned int frameIndex = 0;

    vk::raii::SwapchainKHR vk_swapchain = nullptr;
    std::vector<vk::Image> vk_images;
    std::vector<vk::raii::ImageView> vk_imageViews;

    vk::raii::CommandPool vk_commandPool = nullptr;
    vk::raii::CommandBuffers vk_commandBuffers = nullptr;

    std::vector<vk::raii::Fence> vk_flightFences;
    std::vector<vk::raii::Semaphore> vk_imageSemaphores;
    std::vector<vk::raii::Semaphore> vk_renderSemaphores;
};

} // namespace hlvl