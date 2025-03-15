#include "src/core/include/context.hpp"
#include "src/core/include/materials.hpp"
#include "src/core/include/renderer.hpp"
#include "src/core/include/settings.hpp"
#include "src/core/include/vkfactory.hpp"

#include <algorithm>
#include <limits>

namespace hlvl {

void Renderer::checkFormat() const {
  std::vector<vk::SurfaceFormatKHR> formats = Context::physicalDevice().getSurfaceFormatsKHR(Context::surface());

  for (const auto& format : formats) {
    if (format.format == hlvl_settings.format && format.colorSpace == hlvl_settings.color_space)
      return;
  }

  hlvl_settings.format = formats[0].format;
  hlvl_settings.color_space = formats[0].colorSpace;
}

void Renderer::checkPresentMode() const {
  for (const auto& presentMode : Context::physicalDevice().getSurfacePresentModesKHR(Context::surface())) {
    if (presentMode == hlvl_settings.present_mode)
      return;
  }

  hlvl_settings.present_mode = vk::PresentModeKHR::eFifo;
}

std::pair<unsigned int, vk::SurfaceTransformFlagBitsKHR> Renderer::checkExtent() const {
  vk::SurfaceCapabilitiesKHR surfaceCapabilities = Context::physicalDevice().getSurfaceCapabilitiesKHR(Context::surface());

  unsigned int imageCount = surfaceCapabilities.minImageCount + 1;
  if (imageCount > surfaceCapabilities.maxImageCount && surfaceCapabilities.maxImageCount > 0)
    imageCount = surfaceCapabilities.maxImageCount;

  if (surfaceCapabilities.currentExtent.width != std::numeric_limits<unsigned int>::max()) {
    hlvl_settings.extent = surfaceCapabilities.currentExtent;
    return std::make_pair(imageCount, surfaceCapabilities.currentTransform);
  }

  int width, height;
  glfwGetFramebufferSize(Context::window(), &width, &height);

  hlvl_settings.extent = vk::Extent2D{
    std::clamp(static_cast<unsigned int>(width),
      surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width
    ),
    std::clamp(static_cast<unsigned int>(height),
      surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height
    )
  };

  return std::make_pair(imageCount, surfaceCapabilities.currentTransform);
}

void Renderer::init() {
  checkFormat();
  checkPresentMode();
  auto [imageCount, transform] = checkExtent();

  vk::SwapchainCreateInfoKHR ci_swapchain{
    .surface                = Context::surface(),
    .minImageCount          = imageCount,
    .imageFormat            = hlvl_settings.format,
    .imageColorSpace        = hlvl_settings.color_space,
    .imageExtent            = hlvl_settings.extent,
    .imageArrayLayers       = 1,
    .imageUsage             = vk::ImageUsageFlagBits::eColorAttachment,
    .imageSharingMode       = vk::SharingMode::eExclusive,
    .queueFamilyIndexCount  = 0,
    .pQueueFamilyIndices    = nullptr,
    .preTransform           = transform,
    .compositeAlpha         = vk::CompositeAlphaFlagBitsKHR::eOpaque,
    .presentMode            = hlvl_settings.present_mode,
    .clipped                = true,
    .oldSwapchain           = nullptr
  };

  vk_swapchain = Context::device().createSwapchainKHR(ci_swapchain);
  vk_images = vk_swapchain.getImages();

  createImageViews();

  auto [tmp_commandPool, tmp_commandBuffers] = VulkanFactory::newCommandPool(
    Main, hlvl_settings.buffer_mode, vk::CommandPoolCreateFlagBits::eResetCommandBuffer
  );
  vk_commandPool = std::move(tmp_commandPool);
  vk_commandBuffers = std::move(tmp_commandBuffers);

  createSyncObjects();
}

void Renderer::createImageViews() {
  for (const auto& vk_image : vk_images) {
    vk::ImageViewCreateInfo ci_imageView{
      .image      = vk_image,
      .viewType   = vk::ImageViewType::e2D,
      .format     = hlvl_settings.format,
      .components = {
        .r = vk::ComponentSwizzle::eIdentity,
        .g = vk::ComponentSwizzle::eIdentity,
        .b = vk::ComponentSwizzle::eIdentity,
        .a = vk::ComponentSwizzle::eIdentity
      },
      .subresourceRange = {
        .aspectMask     = vk::ImageAspectFlagBits::eColor,
        .baseMipLevel   = 0,
        .levelCount     = 1,
        .baseArrayLayer = 0,
        .layerCount     = 1
      }
    };

    vk_imageViews.emplace_back(Context::device().createImageView(ci_imageView));
  }
}

void Renderer::createSyncObjects() {
  vk::FenceCreateInfo ci_fence{
    .flags = vk::FenceCreateFlagBits::eSignaled
  };

  vk::SemaphoreCreateInfo ci_semaphore{};

  for (unsigned int i = 0; i < hlvl_settings.buffer_mode; ++i) {
    vk_flightFences.emplace_back(Context::device().createFence(ci_fence));
    vk_imageSemaphores.emplace_back(Context::device().createSemaphore(ci_semaphore));
    vk_renderSemaphores.emplace_back(Context::device().createSemaphore(ci_semaphore));
  }
}

void Renderer::render() {
  static vk::Viewport viewport{
    .x        = 0.0f,
    .y        = 0.0f,
    .width    = static_cast<float>(hlvl_settings.extent.width),
    .height   = static_cast<float>(hlvl_settings.extent.height),
    .minDepth = 0.0f,
    .maxDepth = 1.0f
  };

  static vk::Rect2D scissor{
    .offset = { 0, 0 },
    .extent = hlvl_settings.extent
  };

  static vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;

  if (Context::device().waitForFences(*vk_flightFences[frameIndex], true, 1000000000ul) != vk::Result::eSuccess)
    throw std::runtime_error("hlvl: hung waiting for flight fence");

  auto [res, imgIndex] = vk_swapchain.acquireNextImage(1000000000ul, vk_imageSemaphores[frameIndex], nullptr);
  if (res != vk::Result::eSuccess)
    throw std::runtime_error("hlvl: failed to get next swapchain image");

  Context::device().resetFences(*vk_flightFences[frameIndex]);

  if (hlvl_objects.count() == 0) return;

  beginRendering(imgIndex);

  vk_commandBuffers[frameIndex].setViewport(0, viewport);
  vk_commandBuffers[frameIndex].setScissor(0, scissor);

  for (const auto& object : hlvl_objects)
    renderObject(object);

  endRendering(imgIndex);

  vk::SubmitInfo renderSubmit{
    .waitSemaphoreCount   = 1,
    .pWaitSemaphores      = &*vk_imageSemaphores[frameIndex],
    .pWaitDstStageMask    = &waitStage,
    .commandBufferCount   = 1,
    .pCommandBuffers      = &*vk_commandBuffers[frameIndex],
    .signalSemaphoreCount = 1,
    .pSignalSemaphores    = &*vk_renderSemaphores[frameIndex]
  };

  Context::queue(Main).submit(renderSubmit, vk_flightFences[frameIndex]);

  vk::PresentInfoKHR presentInfo{
    .waitSemaphoreCount = 1,
    .pWaitSemaphores    = &*vk_renderSemaphores[frameIndex],
    .swapchainCount     = 1,
    .pSwapchains        = &*vk_swapchain,
    .pImageIndices      = &imgIndex
  };

  if (Context::queue(Main).presentKHR(presentInfo) != vk::Result::eSuccess)
    throw std::runtime_error("hlvl: failed to present image");

  frameIndex = (frameIndex + 1) % hlvl_settings.buffer_mode;
}

void Renderer::beginRendering(unsigned int imgIndex) {
  vk_commandBuffers[frameIndex].reset();

  vk_commandBuffers[frameIndex].begin(vk::CommandBufferBeginInfo{});

  vk::ImageMemoryBarrier barrier{
    .dstAccessMask    = vk::AccessFlagBits::eColorAttachmentWrite,
    .oldLayout        = vk::ImageLayout::eUndefined,
    .newLayout        = vk::ImageLayout::eColorAttachmentOptimal,
    .image            = vk_images[imgIndex],
    .subresourceRange = {
      .aspectMask     = vk::ImageAspectFlagBits::eColor,
      .baseMipLevel   = 0,
      .levelCount     = 1,
      .baseArrayLayer = 0,
      .layerCount     = 1
    }
  };

  vk_commandBuffers[frameIndex].pipelineBarrier(
    vk::PipelineStageFlagBits::eTopOfPipe,
    vk::PipelineStageFlagBits::eColorAttachmentOutput,
    vk::DependencyFlags(),
    nullptr,
    nullptr,
    barrier
  );

  vk::RenderingAttachmentInfo colorInfo{
    .imageView    = vk_imageViews[imgIndex],
    .imageLayout  = vk::ImageLayout::eColorAttachmentOptimal,
    .loadOp       = vk::AttachmentLoadOp::eClear,
    .storeOp      = vk::AttachmentStoreOp::eStore,
    .clearValue   = { hlvl_settings.background_color }
  };

  vk::RenderingInfo renderInfo{
    .renderArea = {
      .offset = { 0, 0 },
      .extent = hlvl_settings.extent
    },
    .layerCount           = 1,
    .colorAttachmentCount = 1,
    .pColorAttachments    = &colorInfo
  };

  vk_commandBuffers[frameIndex].beginRendering(renderInfo);
}

void Renderer::renderObject(const Object& object) {
  const auto& material = hlvl_materials[object.materialTag];

  vk_commandBuffers[frameIndex].bindPipeline(vk::PipelineBindPoint::eGraphics, material.vk_gPipeline);

  if (!material.vk_descriptorSets.empty()) {
    std::vector<vk::DescriptorSet> sets;
    if (!material.vk_images.empty())
      sets.emplace_back(material.vk_descriptorSets[frameIndex]);

    if (!material.vk_buffers.empty())
      sets.emplace_back(material.vk_descriptorSets[hlvl_settings.buffer_mode * (!material.vk_images.empty()) + frameIndex]);

    vk_commandBuffers[frameIndex].bindDescriptorSets(
      vk::PipelineBindPoint::eGraphics,
      material.vk_gLayout,
      0,
      sets,
      nullptr
    );
  }

  if (material.constants != nullptr) {
    vk_commandBuffers[frameIndex].pushConstants(
      material.vk_gLayout,
      vk::ShaderStageFlagBits::eAllGraphics,
      0,
      vk::ArrayProxy<const char>(material.constantsSize, reinterpret_cast<const char *>(material.constants))
    );
  }

  vk_commandBuffers[frameIndex].bindVertexBuffers(0, *object.vk_buffers[0], { 0 });
  vk_commandBuffers[frameIndex].bindIndexBuffer(*object.vk_buffers[1], 0, vk::IndexType::eUint32);

  vk_commandBuffers[frameIndex].drawIndexed(object.indexCount, 1, 0, 0, 0);
}

void Renderer::endRendering(unsigned int imgIndex) {
  vk_commandBuffers[frameIndex].endRendering();

  vk::ImageMemoryBarrier barrier{
    .srcAccessMask    = vk::AccessFlagBits::eColorAttachmentWrite,
    .oldLayout        = vk::ImageLayout::eColorAttachmentOptimal,
    .newLayout        = vk::ImageLayout::ePresentSrcKHR,
    .image            = vk_images[imgIndex],
    .subresourceRange = {
      .aspectMask     = vk::ImageAspectFlagBits::eColor,
      .baseMipLevel   = 0,
      .levelCount     = 1,
      .baseArrayLayer = 0,
      .layerCount     = 1
    }
  };

  vk_commandBuffers[frameIndex].pipelineBarrier(
    vk::PipelineStageFlagBits::eColorAttachmentOutput,
    vk::PipelineStageFlagBits::eBottomOfPipe,
    vk::DependencyFlags(),
    nullptr,
    nullptr,
    barrier
  );

  vk_commandBuffers[frameIndex].end();
}

} // namespace hlvl