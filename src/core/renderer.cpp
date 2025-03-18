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

  auto [tmp_dMemory, tmp_dImages, tmp_dViews] = VulkanFactory::newDepthAllocation(vk_images.size());
  vk_dMemory = std::move(tmp_dMemory);
  vk_dImages = std::move(tmp_dImages);
  vk_dViews = std::move(tmp_dViews);

  auto [tmp_commandPool, tmp_commandBuffers] = VulkanFactory::newCommandPool(
    Main, hlvl_settings.buffer_mode, vk::CommandPoolCreateFlagBits::eResetCommandBuffer
  );
  vk_commandPool = std::move(tmp_commandPool);
  vk_commandBuffers = std::move(tmp_commandBuffers);

  auto [tmp_computePool, tmp_computeBuffers] = VulkanFactory::newCommandPool(
    Compute, hlvl_settings.buffer_mode, vk::CommandPoolCreateFlagBits::eResetCommandBuffer
  );
  vk_computePool = std::move(tmp_computePool);
  vk_computeBuffers = std::move(tmp_computeBuffers);

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
    vk_computeFences.emplace_back(Context::device().createFence(ci_fence));
    vk_imageSemaphores.emplace_back(Context::device().createSemaphore(ci_semaphore));
    vk_renderSemaphores.emplace_back(Context::device().createSemaphore(ci_semaphore));
    vk_computeSemaphores.emplace_back(Context::device().createSemaphore(ci_semaphore));
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

  static vk::PipelineStageFlags waitStages[2] = { vk::PipelineStageFlagBits::eVertexShader, vk::PipelineStageFlagBits::eColorAttachmentOutput };

  if (Context::device().waitForFences({ *vk_flightFences[frameIndex], *vk_computeFences[frameIndex] }, true, 1000000000ul) != vk::Result::eSuccess)
    throw std::runtime_error("hlvl: hung waiting for flight fence");

  auto [res, imgIndex] = vk_swapchain.acquireNextImage(1000000000ul, vk_imageSemaphores[frameIndex], nullptr);
  if (res != vk::Result::eSuccess)
    throw std::runtime_error("hlvl: failed to get next swapchain image");

  Context::device().resetFences({ *vk_flightFences[frameIndex], *vk_computeFences[frameIndex] });

  if (hlvl_objects.count() == 0) return;

  beginRendering(imgIndex);

  vk_commandBuffers[frameIndex].setViewport(0, viewport);
  vk_commandBuffers[frameIndex].setScissor(0, scissor);

  for (const auto& object : hlvl_objects)
    renderObject(object);

  endRendering(imgIndex);

  vk::SubmitInfo computeSubmit{
    .commandBufferCount   = 1,
    .pCommandBuffers      = &*vk_computeBuffers[frameIndex],
    .signalSemaphoreCount = 1,
    .pSignalSemaphores    = &*vk_computeSemaphores[frameIndex]
  };

  Context::queue(Compute).submit(computeSubmit, vk_computeFences[frameIndex]);

  vk::Semaphore waitSemaphores[2] = { *vk_computeSemaphores[frameIndex], *vk_imageSemaphores[frameIndex] };
  vk::SubmitInfo renderSubmit{
    .waitSemaphoreCount   = 2,
    .pWaitSemaphores      = waitSemaphores,
    .pWaitDstStageMask    = waitStages,
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
  vk_computeBuffers[frameIndex].reset();

  vk_commandBuffers[frameIndex].begin(vk::CommandBufferBeginInfo{});
  vk_computeBuffers[frameIndex].begin(vk::CommandBufferBeginInfo{});

  vk::ImageMemoryBarrier colorBarrier{
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
    colorBarrier
  );

  vk::ImageMemoryBarrier depthBarrier{
    .oldLayout  = vk::ImageLayout::eUndefined,
    .newLayout  = vk::ImageLayout::eDepthStencilAttachmentOptimal,
    .image      = vk_dImages[imgIndex],
    .subresourceRange = {
      .aspectMask     = vk::ImageAspectFlagBits::eDepth,
      .baseMipLevel   = 0,
      .levelCount     = 1,
      .baseArrayLayer = 0,
      .layerCount     = 1
    }
  };

  vk_commandBuffers[frameIndex].pipelineBarrier(
    vk::PipelineStageFlagBits::eTopOfPipe,
    vk::PipelineStageFlagBits::eEarlyFragmentTests,
    vk::DependencyFlags(),
    nullptr,
    nullptr,
    depthBarrier
  );

  vk::RenderingAttachmentInfo colorInfo{
    .imageView    = vk_imageViews[imgIndex],
    .imageLayout  = vk::ImageLayout::eColorAttachmentOptimal,
    .loadOp       = vk::AttachmentLoadOp::eClear,
    .storeOp      = vk::AttachmentStoreOp::eStore,
    .clearValue   = { hlvl_settings.background_color }
  };

  vk::RenderingAttachmentInfo depthInfo{
    .imageView    = vk_dViews[imgIndex],
    .imageLayout  = vk::ImageLayout::eDepthAttachmentOptimal,
    .loadOp       = vk::AttachmentLoadOp::eClear,
    .storeOp      = vk::AttachmentStoreOp::eDontCare,
    .clearValue   = { .depthStencil = { 1, 0 } }
  };

  vk::RenderingInfo renderInfo{
    .renderArea = {
      .offset = { 0, 0 },
      .extent = hlvl_settings.extent
    },
    .layerCount           = 1,
    .colorAttachmentCount = 1,
    .pColorAttachments    = &colorInfo,
    .pDepthAttachment     = &depthInfo
  };

  vk_commandBuffers[frameIndex].beginRendering(renderInfo);
}

void Renderer::renderObject(const Object& object) {
  const auto& material = hlvl_materials[object.materialTag];

  if (material.hasCanvas) {
    for (unsigned int i = material.canvasIndex; i < material.vk_images.size(); ++i) {
      vk::ImageMemoryBarrier barrier{
        .srcAccessMask    = vk::AccessFlagBits::eShaderRead,
        .dstAccessMask    = vk::AccessFlagBits::eShaderWrite,
        .newLayout        = vk::ImageLayout::eGeneral,
        .image            = material.vk_images[i],
        .subresourceRange = {
          .aspectMask     = vk::ImageAspectFlagBits::eColor,
          .baseMipLevel   = 0,
          .levelCount     = 1,
          .baseArrayLayer = 0,
          .layerCount     = 1
        }
      };

      vk_computeBuffers[frameIndex].pipelineBarrier(
        vk::PipelineStageFlagBits::eFragmentShader,
        vk::PipelineStageFlagBits::eComputeShader,
        vk::DependencyFlags(),
        nullptr,
        nullptr,
        barrier
      );

      barrier.srcAccessMask = vk::AccessFlagBits::eShaderWrite;
      barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
      barrier.oldLayout = vk::ImageLayout::eGeneral;
      barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

      vk_computeBuffers[frameIndex].pipelineBarrier(
        vk::PipelineStageFlagBits::eComputeShader,
        vk::PipelineStageFlagBits::eFragmentShader,
        vk::DependencyFlags(),
        nullptr,
        nullptr,
        barrier
      );
    }
  }

  vk_commandBuffers[frameIndex].bindPipeline(vk::PipelineBindPoint::eGraphics, material.vk_gPipeline);

  if (*material.vk_cPipeline != nullptr)
    vk_computeBuffers[frameIndex].bindPipeline(vk::PipelineBindPoint::eCompute, material.vk_cPipeline);

  std::vector<vk::DescriptorSet> sets;
  for (const auto& set : material.vk_descriptorSets)
    sets.emplace_back(set[frameIndex]);

  if (!sets.empty()) {
    vk_commandBuffers[frameIndex].bindDescriptorSets(
      vk::PipelineBindPoint::eGraphics, material.vk_Layout, 0, sets, nullptr
    );

    if (*material.vk_cPipeline != nullptr) {
      vk_computeBuffers[frameIndex].bindDescriptorSets(
        vk::PipelineBindPoint::eCompute, material.vk_Layout, 0, sets, nullptr
      );
    }
  }

  if (material.constants != nullptr) {
    vk_commandBuffers[frameIndex].pushConstants(
      material.vk_Layout,
      vk::ShaderStageFlagBits::eAll,
      0,
      vk::ArrayProxy<const char>(material.constantsSize, reinterpret_cast<const char *>(material.constants))
    );

    if (*material.vk_cPipeline != nullptr) {
      vk_computeBuffers[frameIndex].pushConstants(
        material.vk_Layout,
        vk::ShaderStageFlagBits::eAll,
        0,
        vk::ArrayProxy<const char>(material.constantsSize, reinterpret_cast<const char *>(material.constants))
      );
    }
  }

  vk_commandBuffers[frameIndex].bindVertexBuffers(0, *object.vk_buffers[0], { 0 });
  vk_commandBuffers[frameIndex].bindIndexBuffer(*object.vk_buffers[1], 0, vk::IndexType::eUint32);

  vk_commandBuffers[frameIndex].drawIndexed(object.indexCount, 1, 0, 0, 0);

  if (*material.vk_cPipeline != nullptr) {
    vk_computeBuffers[frameIndex].dispatch(
      material.computeSpace[0], material.computeSpace[1], material.computeSpace[2]
    );
  }
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
  vk_computeBuffers[frameIndex].end();
}

} // namespace hlvl