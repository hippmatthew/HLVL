#include "src/core/include/settings.hpp"
#include "src/core/include/vkfactory.hpp"

#include <png.h>

#include <cstdio>
#include <cstdlib>
#include <queue>

namespace hlvl {

VulkanFactory::AllocationOutput VulkanFactory::newAllocation(
  const std::vector<vk::BufferCreateInfo>& bufferInfos,
  vk::MemoryPropertyFlags flags
) {
  vk::raii::DeviceMemory memory = nullptr;
  std::vector<vk::raii::Buffer> buffers;
  std::vector<unsigned int> offsets;
  unsigned int allocationSize = 0;
  unsigned int filter = ~(0x0);

  for (const auto& info : bufferInfos)
    buffers.emplace_back(Context::device().createBuffer(info));

  for (const auto& buffer : buffers) {
    vk::MemoryRequirements requirements = buffer.getMemoryRequirements();

    unsigned int offset =
      (allocationSize + requirements.alignment - 1) / requirements.alignment * requirements.alignment;
    offsets.emplace_back(offset);

    filter &= requirements.memoryTypeBits;
    allocationSize = offset + requirements.size;
  }

  vk::MemoryAllocateInfo allocateInfo{
    .allocationSize   = allocationSize,
    .memoryTypeIndex  = findMemoryIndex(filter, flags)
  };

  memory = Context::device().allocateMemory(allocateInfo);

  for (unsigned int i = 0; i < buffers.size(); ++i)
    buffers[i].bindMemory(memory, offsets[i]);

  return { std::move(memory), std::move(buffers), std::move(offsets), std::move(allocationSize) };
}

VulkanFactory::CommandPoolOutput VulkanFactory::newCommandPool(
  QueueFamilyType queueType,
  unsigned int count,
  vk::CommandPoolCreateFlags flags
) {
  vk::raii::CommandPool pool = nullptr;
  vk::raii::CommandBuffers buffers = nullptr;

  vk::CommandPoolCreateInfo ci_pool{
    .flags            = flags,
    .queueFamilyIndex = Context::queueIndex(queueType)
  };
  pool = Context::device().createCommandPool(ci_pool);

  vk::CommandBufferAllocateInfo allocateInfo{
    .commandPool        = pool,
    .level              = vk::CommandBufferLevel::ePrimary,
    .commandBufferCount = count
  };
  buffers = vk::raii::CommandBuffers(Context::device(), allocateInfo);

  return { std::move(pool), std::move(buffers) };
}

VulkanFactory::DescriptorPoolOutput VulkanFactory::newDescriptorPool(
  vk::DescriptorPoolCreateFlags flags,
  const std::vector<vk::raii::DescriptorSetLayout>& vk_dsLayouts,
  unsigned int texCount,
  unsigned int sCount,
  unsigned int uCount
) {
  vk::raii::DescriptorPool pool = nullptr;
  std::vector<vk::raii::DescriptorSets> sets;

  std::vector<vk::DescriptorPoolSize> poolSizes;

  if (texCount != 0) {
    poolSizes.emplace_back(vk::DescriptorPoolSize{
      .type             = vk::DescriptorType::eCombinedImageSampler,
      .descriptorCount  = texCount * hlvl_settings.buffer_mode
    });
  }

  if (sCount != 0) {
    poolSizes.emplace_back(vk::DescriptorPoolSize{
      .type             = vk::DescriptorType::eStorageBuffer,
      .descriptorCount  = sCount * hlvl_settings.buffer_mode
    });
  }

  if (uCount != 0) {
    poolSizes.emplace_back(vk::DescriptorPoolSize{
      .type             = vk::DescriptorType::eUniformBuffer,
      .descriptorCount  = uCount * hlvl_settings.buffer_mode
    });
  }

  vk::DescriptorPoolCreateInfo ci_pool{
    .flags          = flags,
    .maxSets        = static_cast<unsigned int>(hlvl_settings.buffer_mode * ((texCount != 0) + (sCount != 0) + (uCount != 0))),
    .poolSizeCount  = static_cast<unsigned int>(poolSizes.size()),
    .pPoolSizes     = poolSizes.data()
  };

  pool = Context::device().createDescriptorPool(ci_pool);

  for (const auto& layout : vk_dsLayouts) {
    std::vector<vk::DescriptorSetLayout> layouts;
    for (unsigned int i = 0; i < hlvl_settings.buffer_mode; ++i)
      layouts.emplace_back(layout);

    sets.emplace_back(vk::raii::DescriptorSets(Context::device(), vk::DescriptorSetAllocateInfo{
      .descriptorPool     = pool,
      .descriptorSetCount = hlvl_settings.buffer_mode,
      .pSetLayouts        = layouts.data()
    }));
  }

  return { std::move(pool), std::move(sets) };
}

VulkanFactory::TextureOutput VulkanFactory::newTextureAllocation(const std::vector<std::string>& paths) {
  vk::raii::DeviceMemory memory = nullptr;
  std::vector<vk::raii::Image> images;
  std::vector<vk::raii::ImageView> views;
  std::vector<vk::raii::Sampler> samplers;

  std::vector<vk::BufferCreateInfo> bufferInfos;
  std::vector<PNG> imgs;

  for (const auto& path : paths) {
    auto [data, width, height, rowSize] = readPNG(path);
    imgs.emplace_back(std::tuple{ data, width, height, rowSize });

    bufferInfos.emplace_back(vk::BufferCreateInfo{
      .size         = height * rowSize,
      .usage        = vk::BufferUsageFlagBits::eTransferSrc,
      .sharingMode  = vk::SharingMode::eExclusive
    });

    images.emplace_back(Context::device().createImage(vk::ImageCreateInfo{
      .imageType  = vk::ImageType::e2D,
      .format     = vk::Format::eR8G8B8A8Srgb,
      .extent     = {
        .width  = width,
        .height = height,
        .depth  = 1
      },
      .mipLevels    = 1,
      .arrayLayers  = 1,
      .samples      = vk::SampleCountFlagBits::e1,
      .tiling       = vk::ImageTiling::eOptimal,
      .usage        = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst,
      .sharingMode  = vk::SharingMode::eExclusive
    }));
  }

  auto [stagingMemory, stagingBuffers, stagingOffsets, stagingSize] = newAllocation(
    bufferInfos, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
  );

  void * memoryMap = stagingMemory.mapMemory(0, stagingSize);

  for (unsigned int i = 0; i < stagingBuffers.size(); ++i) {
    auto [data, _, height, rowSize] = imgs[i];
    memcpy((unsigned char *)memoryMap + stagingOffsets[i], data, height * rowSize);
    free(data);
  }

  stagingMemory.unmapMemory();
  memoryMap = nullptr;

  unsigned int filter = ~(0x0);
  std::vector<unsigned int> offsets;
  unsigned int allocationSize = 0;

  for (const auto& image : images) {
    vk::MemoryRequirements requirements = image.getMemoryRequirements();

    unsigned int offset =
      (allocationSize + requirements.alignment - 1) / requirements.alignment * requirements.alignment;
    offsets.emplace_back(offset);

    filter &= requirements.memoryTypeBits;
    allocationSize = offset + requirements.size;
  }

  memory = Context::device().allocateMemory(vk::MemoryAllocateInfo{
    .allocationSize   = allocationSize,
    .memoryTypeIndex  = findMemoryIndex(filter, vk::MemoryPropertyFlagBits::eDeviceLocal)
  });

  for (unsigned int i = 0; i < images.size(); ++i)
    images[i].bindMemory(memory, offsets[i]);

  auto [commandPool, commandBuffers] = newCommandPool(
    Transfer, images.size(), vk::CommandPoolCreateFlagBits::eTransient
  );

  unsigned int index = 0;
  for (auto& commandBuffer : commandBuffers) {
    commandBuffer.begin(vk::CommandBufferBeginInfo{
      .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit
    });

    vk::ImageMemoryBarrier barrier{
      .srcAccessMask    = vk::AccessFlagBits::eNone,
      .dstAccessMask    = vk::AccessFlagBits::eTransferWrite,
      .oldLayout        = vk::ImageLayout::eUndefined,
      .newLayout        = vk::ImageLayout::eTransferDstOptimal,
      .image            = images[index],
      .subresourceRange = {
        .aspectMask     = vk::ImageAspectFlagBits::eColor,
        .baseMipLevel   = 0,
        .levelCount     = 1,
        .baseArrayLayer = 0,
        .layerCount     = 1
      }
    };

    commandBuffer.pipelineBarrier(
      vk::PipelineStageFlagBits::eTopOfPipe,
      vk::PipelineStageFlagBits::eTransfer,
      vk::DependencyFlags(),
      nullptr,
      nullptr,
      barrier
    );

    auto [_, width, height, __] = imgs[index];
    vk::BufferImageCopy imageCopy{
      .bufferOffset       = 0,
      .bufferRowLength    = 0,
      .bufferImageHeight  = 0,
      .imageSubresource   = {
        .aspectMask     = vk::ImageAspectFlagBits::eColor,
        .mipLevel       = 0,
        .baseArrayLayer = 0,
        .layerCount     = 1,
      },
      .imageOffset = { 0, 0 },
      .imageExtent = {
        .width  = width,
        .height = height,
        .depth  = 1
      }
    };

    commandBuffer.copyBufferToImage(stagingBuffers[index], images[index], vk::ImageLayout::eTransferDstOptimal, imageCopy);

    barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
    barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
    barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
    barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

    commandBuffer.pipelineBarrier(
      vk::PipelineStageFlagBits::eTransfer,
      vk::PipelineStageFlagBits::eFragmentShader,
      vk::DependencyFlags(),
      nullptr,
      nullptr,
      barrier
    );

    commandBuffer.end();
  }

  std::vector<vk::CommandBuffer> cmds;
  for (const auto& commandBuffer : commandBuffers)
    cmds.emplace_back(*commandBuffer);

  vk::SubmitInfo submit{
    .commandBufferCount = static_cast<unsigned int>(cmds.size()),
    .pCommandBuffers    = cmds.data()
  };

  vk::raii::Fence vk_fence = Context::device().createFence(vk::FenceCreateInfo{});
  Context::queue(Transfer).submit(submit, vk_fence);

  if (Context::device().waitForFences(*vk_fence, true, 1000000000ul) != vk::Result::eSuccess)
    throw std::runtime_error("hlvl: hung waiting for image transfer");

  for (const auto& image : images) {
    views.emplace_back(Context::device().createImageView(vk::ImageViewCreateInfo{
      .image      = image,
      .viewType   = vk::ImageViewType::e2D,
      .format     = vk::Format::eR8G8B8A8Srgb,
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
    }));

    vk::PhysicalDeviceProperties properties = Context::physicalDevice().getProperties();

    samplers.emplace_back(Context::device().createSampler(vk::SamplerCreateInfo{
      .magFilter                = vk::Filter::eLinear,
      .minFilter                = vk::Filter::eLinear,
      .mipmapMode               = vk::SamplerMipmapMode::eLinear,
      .addressModeU             = vk::SamplerAddressMode::eRepeat,
      .addressModeV             = vk::SamplerAddressMode::eRepeat,
      .addressModeW             = vk::SamplerAddressMode::eRepeat,
      .mipLodBias               = 0,
      .anisotropyEnable         = true,
      .maxAnisotropy            = properties.limits.maxSamplerAnisotropy,
      .compareEnable            = false,
      .minLod                   = 0,
      .maxLod                   = 0,
      .borderColor              = vk::BorderColor::eIntOpaqueBlack,
      .unnormalizedCoordinates  = false
    }));
  }

  return { std::move(memory), std::move(images), std::move(views), std::move(samplers) };
}

VulkanFactory::DepthOutput VulkanFactory::newDepthAllocation(unsigned int imageCount) {
  vk::raii::DeviceMemory memory = nullptr;
  std::vector<vk::raii::Image> images;
  std::vector<vk::raii::ImageView> views;

  std::queue<vk::Format> depthFormats;
  depthFormats.emplace(vk::Format::eD32SfloatS8Uint);
  depthFormats.emplace(vk::Format::eD24UnormS8Uint);

  bool supported = false;
  while (!depthFormats.empty()) {
    vk::FormatProperties properties = Context::physicalDevice().getFormatProperties(hlvl_settings.depth_format);

    if (properties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment) {
      supported = true;
      break;
    }

    hlvl_settings.depth_format = depthFormats.front();
    depthFormats.pop();
  }
  if (!supported)
    throw std::runtime_error("hlvl: failed to find supported depth format");

  for (unsigned int i = 0; i < imageCount; ++i) {
    vk::ImageCreateInfo ci_image{
      .imageType  = vk::ImageType::e2D,
      .format     = hlvl_settings.depth_format,
      .extent     = {
        .width  = hlvl_settings.extent.width,
        .height = hlvl_settings.extent.height,
        .depth  = 1
      },
      .mipLevels    = 1,
      .arrayLayers  = 1,
      .samples      = vk::SampleCountFlagBits::e1,
      .tiling       = vk::ImageTiling::eOptimal,
      .usage        = vk::ImageUsageFlagBits::eDepthStencilAttachment,
      .sharingMode  = vk::SharingMode::eExclusive
    };

    images.emplace_back(Context::device().createImage(ci_image));
  }

  unsigned int filter = ~(0x0);
  unsigned int allocationSize = 0;
  std::vector<unsigned int> offsets;

  for (const auto& image : images) {
    vk::MemoryRequirements requirements = image.getMemoryRequirements();

    unsigned int offset =
      (allocationSize + requirements.alignment - 1) / requirements.alignment * requirements.alignment;

    offsets.emplace_back(offset);

    filter &= requirements.memoryTypeBits;
    allocationSize = offset + requirements.size;
  }

  vk::MemoryAllocateInfo allocateInfo{
    .allocationSize   = allocationSize,
    .memoryTypeIndex  = findMemoryIndex(filter, vk::MemoryPropertyFlagBits::eDeviceLocal)
  };

  memory = Context::device().allocateMemory(allocateInfo);

  for (unsigned int i = 0; i < imageCount; ++i)
    images[i].bindMemory(memory, offsets[i]);

  for (const auto& image : images) {
    vk::ImageViewCreateInfo ci_view{
      .image      = image,
      .viewType   = vk::ImageViewType::e2D,
      .format     = hlvl_settings.depth_format,
      .components = {
        .r = vk::ComponentSwizzle::eIdentity,
        .g = vk::ComponentSwizzle::eIdentity,
        .b = vk::ComponentSwizzle::eIdentity,
        .a = vk::ComponentSwizzle::eIdentity
      },
      .subresourceRange = {
        .aspectMask     = vk::ImageAspectFlagBits::eDepth,
        .baseMipLevel   = 0,
        .levelCount     = 1,
        .baseArrayLayer = 0,
        .layerCount     = 1
      }
    };

    views.emplace_back(Context::device().createImageView(ci_view));
  }


  return { std::move(memory), std::move(images), std::move(views) };
}

unsigned int VulkanFactory::findMemoryIndex(unsigned int filter, vk::MemoryPropertyFlags flags) {
  vk::PhysicalDeviceMemoryProperties properties = Context::physicalDevice().getMemoryProperties();

  unsigned int index = 0;
  for (const auto& memoryType : properties.memoryTypes) {
    if (index == properties.memoryTypeCount) break;

    if ((filter & (1 << index)) && ((memoryType.propertyFlags & flags) == flags))
      return index;
    ++index;
  }

  throw std::runtime_error("hlvl: failed to find a suitable memory index for buffer memory allocation");
}

VulkanFactory::PNG VulkanFactory::readPNG(const std::string& path) {
  FILE * image = fopen(path.c_str(), "rb");
  if (!image) throw std::runtime_error("hlvl: failed to open image");

  unsigned char header[8];
  fread(header, 1, 8, image);
  if (png_sig_cmp(header, 0, 8)) {
    fclose(image);
    throw std::runtime_error("hlvl: unknown image file type");
  }

  png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (!png) {
    fclose(image);
    throw std::runtime_error("hlvl: failed during png initialization");
  }

  png_infop info = png_create_info_struct(png);
  if (!info) {
    fclose(image);
    png_destroy_read_struct(&png, nullptr, nullptr);
    throw std::runtime_error("hlvl: failed during png initialization");
  }

  if (setjmp(png_jmpbuf(png))) {
    fclose(image);
    png_destroy_read_struct(&png, &info, nullptr);
    throw std::runtime_error("hlvl: failed during png initialization");
  }

  png_init_io(png, image);
  png_set_sig_bytes(png, 8);

  png_read_info(png, info);

  unsigned int width, height;
  int bitDepth, colorType;
  png_get_IHDR(png, info, &width, &height, &bitDepth, &colorType, nullptr, nullptr, nullptr);

  if (colorType == PNG_COLOR_TYPE_PALETTE)
    png_set_palette_to_rgb(png);
  else if (colorType == PNG_COLOR_TYPE_GRAY && bitDepth < 8)
    png_set_expand_gray_1_2_4_to_8(png);

  if (png_get_valid(png, info, PNG_INFO_tRNS))
    png_set_tRNS_to_alpha(png);

  if (bitDepth == 16)
    png_set_strip_16(png);

  png_read_update_info(png, info);

  unsigned int rowSize = png_get_rowbytes(png, info);
  unsigned char ** data = (unsigned char **)malloc(height * sizeof(unsigned char *));
  if (!data) {
    fclose(image);
    png_destroy_read_struct(&png, &info, nullptr);
    throw std::runtime_error("hlvl: failed to allocate space for image");
  }

  for (int i = 0; i < height; ++i) {
    data[i] = (unsigned char *)malloc(rowSize);
    if (!data[i]) {
      free(data);
      fclose(image);
      png_destroy_read_struct(&png, &info, nullptr);
      throw std::runtime_error("hlvl: failed to allocate space for image");
    }
  }

  png_read_image(png, data);

  png_destroy_read_struct(&png, &info, nullptr);
  fclose(image);

  unsigned char * packedData = (unsigned char *)malloc(height * rowSize);
  for (unsigned int i = 0; i < height; ++i) {
    memcpy(packedData + i * rowSize, data[i], rowSize);
    free(data[i]);
  }
  free(data);

  return { packedData, width, height, rowSize };
}

} // namespace hlvl
