#include "src/core/include/context.hpp"
#include "src/core/include/materials.hpp"
#include "src/core/include/settings.hpp"
#include "src/core/include/vertex.hpp"
#include "src/core/include/vkfactory.hpp"

#include <fstream>

namespace hlvl {

Materials * Materials::p_materials = nullptr;

Material::MaterialBuilder::MaterialBuilder(std::string t) {
  tag = t;
}

Material::MaterialBuilder& Material::MaterialBuilder::add_shader(vk::ShaderStageFlagBits stage, std::string path) {
  shaderMap[stage] = path;
  return *this;
}

Material::MaterialBuilder& Material::MaterialBuilder::add_texture(std::string path) {
  textures.emplace_back(path);
  return *this;
}

Material::MaterialBuilder& Material::MaterialBuilder::add_storage(vk::ShaderStageFlagBits stages, ResourceProxy * resource) {
  sResources.emplace_back(std::make_pair(stages, resource));
  return *this;
}

Material::MaterialBuilder& Material::MaterialBuilder::add_uniform(vk::ShaderStageFlagBits stages, ResourceProxy * resource) {
  uResources.emplace_back(std::make_pair(stages, resource));
  return *this;
}

Material::MaterialBuilder& Material::MaterialBuilder::add_constants(unsigned int s, void * d) {
  constantsSize = s;
  constants = d;
  return *this;
}

Material::Material(MaterialBuilder& materialBuilder) {
  createLayout(materialBuilder);
  createGraphicsPipeline(materialBuilder);

  if (!materialBuilder.textures.empty())
    createTextureDescriptors(materialBuilder);

  if (!materialBuilder.sResources.empty())
    createStorageDescriptors(materialBuilder);

  if (!materialBuilder.uResources.empty())
    createUniformDescriptors(materialBuilder);

  if (!(vk_images.empty() && vk_sBuffers.empty()))
    createDescriptorSets(materialBuilder);

  constantsSize = materialBuilder.constantsSize;
  constants = materialBuilder.constants;
}

Material::MaterialBuilder Material::builder(std::string tag) {
  return MaterialBuilder(tag);
}

std::vector<char> Material::read(std::string path) const {
  std::vector<char> buffer;

  std::ifstream shader(path, std::ios::binary | std::ios::ate);
  if (!shader) throw std::runtime_error("hlvl: failed to parse shader");

  unsigned long size = shader.tellg();
  buffer.resize(size);

  if (size < 4)
    throw std::runtime_error("hlvl: shader file too small");

  if (size % 4 != 0)
    throw std::runtime_error("hlvl: shader file size not a multiple of 4");

  shader.seekg(0);
  shader.read(buffer.data(), 4);

  if (*reinterpret_cast<unsigned int *>(buffer.data()) != 0x07230203)
    throw std::runtime_error("hlvl: invalid shader file format");

  shader.read(buffer.data() + 4, size - 4);

  return buffer;
}

std::pair<std::vector<vk::raii::ShaderModule>, std::vector<vk::PipelineShaderStageCreateInfo>>
Material::processShaders(MaterialBuilder& b) const {
  std::vector<vk::raii::ShaderModule> modules;
  std::vector<vk::PipelineShaderStageCreateInfo> infos;

  for (auto& [stage, path] : b.shaderMap) {
    std::vector<char> code = read(path);
    vk::ShaderModuleCreateInfo ci_module{
      .codeSize = static_cast<unsigned int>(code.size()),
      .pCode    = reinterpret_cast<unsigned int *>(code.data())
    };

    modules.emplace_back(Context::device().createShaderModule(ci_module));

    infos.emplace_back(vk::PipelineShaderStageCreateInfo{
      .stage  = stage,
      .module = modules[modules.size() - 1],
      .pName  = "main"
    });
  }

  return std::make_pair(std::move(modules), std::move(infos));
}

void Material::createLayout(MaterialBuilder& materialBuilder) {
  if (!materialBuilder.textures.empty()) {
    std::vector<vk::DescriptorSetLayoutBinding> bindings;

    for (unsigned int i = 0; i < materialBuilder.textures.size(); ++i) {
      bindings.emplace_back(vk::DescriptorSetLayoutBinding{
        .binding          = i,
        .descriptorType   = vk::DescriptorType::eCombinedImageSampler,
        .descriptorCount  = 1,
        .stageFlags       = vk::ShaderStageFlagBits::eFragment
      });
    }

    vk_dsLayouts.emplace_back(Context::device().createDescriptorSetLayout(vk::DescriptorSetLayoutCreateInfo{
      .bindingCount = static_cast<unsigned int>(bindings.size()),
      .pBindings    = bindings.data()
    }));
  }

  if (!materialBuilder.sResources.empty()) {
    std::vector<vk::DescriptorSetLayoutBinding> bindings;

    for (unsigned int i = 0; i < materialBuilder.sResources.size(); ++i) {
      bindings.emplace_back(vk::DescriptorSetLayoutBinding{
        .binding          = i,
        .descriptorType   = vk::DescriptorType::eStorageBuffer,
        .descriptorCount  = 1,
        .stageFlags       = materialBuilder.sResources[i].first
      });
    }

    vk_dsLayouts.emplace_back(Context::device().createDescriptorSetLayout(vk::DescriptorSetLayoutCreateInfo{
      .bindingCount = static_cast<unsigned int>(bindings.size()),
      .pBindings    = bindings.data()
    }));
  }

  if (!materialBuilder.uResources.empty()) {
    std::vector<vk::DescriptorSetLayoutBinding> bindings;

    for (unsigned int i = 0; i < materialBuilder.uResources.size(); ++i) {
      bindings.emplace_back(vk::DescriptorSetLayoutBinding{
        .binding          = i,
        .descriptorType   = vk::DescriptorType::eUniformBuffer,
        .descriptorCount  = 1,
        .stageFlags       = materialBuilder.uResources[i].first
      });
    }

    vk_dsLayouts.emplace_back(Context::device().createDescriptorSetLayout(vk::DescriptorSetLayoutCreateInfo{
      .bindingCount = static_cast<unsigned int>(bindings.size()),
      .pBindings    = bindings.data()
    }));
  }

  vk::PushConstantRange pushConstants{
    .stageFlags = vk::ShaderStageFlagBits::eAllGraphics | vk::ShaderStageFlagBits::eCompute,
    .size       = materialBuilder.constantsSize
  };

  std::vector<vk::DescriptorSetLayout> layouts;
  for (const auto& vk_dsLayout : vk_dsLayouts)
    layouts.emplace_back(vk_dsLayout);

  vk_Layout = Context::device().createPipelineLayout(vk::PipelineLayoutCreateInfo{
    .setLayoutCount         = static_cast<unsigned int>(layouts.size()),
    .pSetLayouts            = layouts.data(),
    .pushConstantRangeCount = materialBuilder.constantsSize != 0,
    .pPushConstantRanges    = &pushConstants
  });
}

void Material::createGraphicsPipeline(MaterialBuilder& materialBuilder) {
    auto [modules, ci_stages] = processShaders(materialBuilder);

  vk::DynamicState dynamicStates[2] = {
    vk::DynamicState::eViewport,
    vk::DynamicState::eScissor
  };

  vk::PipelineDynamicStateCreateInfo ci_dynamicState{
    .dynamicStateCount  = 2,
    .pDynamicStates     = dynamicStates
  };

  vk::PipelineViewportStateCreateInfo ci_viewport{
    .viewportCount  = 1,
    .scissorCount   = 1
  };

  auto binding = Vertex::binding();
  auto attributes = Vertex::attributes();

  vk::PipelineVertexInputStateCreateInfo ci_inputState{
    .vertexBindingDescriptionCount    = 1,
    .pVertexBindingDescriptions       = &binding,
    .vertexAttributeDescriptionCount  = static_cast<unsigned int>(attributes.size()),
    .pVertexAttributeDescriptions     = attributes.data()
  };

  vk::PipelineInputAssemblyStateCreateInfo ci_assembly{
    .topology               = vk::PrimitiveTopology::eTriangleList,
    .primitiveRestartEnable = false
  };

  vk::PipelineRasterizationStateCreateInfo ci_rasterizer{
    .depthClampEnable         = false,
    .rasterizerDiscardEnable  = false,
    .polygonMode              = vk::PolygonMode::eFill,
    .cullMode                 = vk::CullModeFlagBits::eBack,
    .frontFace                = vk::FrontFace::eCounterClockwise,
    .depthBiasEnable          = false,
    .lineWidth                = 1.0f
  };

  vk::PipelineMultisampleStateCreateInfo ci_multisample{
    .rasterizationSamples = vk::SampleCountFlagBits::e1,
    .sampleShadingEnable  = false
  };

  vk::PipelineDepthStencilStateCreateInfo ci_depth{
    .depthTestEnable        = true,
    .depthWriteEnable       = true,
    .depthCompareOp         = vk::CompareOp::eLess,
    .depthBoundsTestEnable  = false,
    .stencilTestEnable      = false
  };

  vk::PipelineColorBlendAttachmentState colorAttachment{
    .blendEnable          = true,
    .srcColorBlendFactor  = vk::BlendFactor::eSrcAlpha,
    .dstColorBlendFactor  = vk::BlendFactor::eOneMinusSrcAlpha,
    .colorBlendOp         = vk::BlendOp::eAdd,
    .srcAlphaBlendFactor  = vk::BlendFactor::eOne,
    .dstAlphaBlendFactor  = vk::BlendFactor::eOneMinusSrcAlpha,
    .alphaBlendOp         = vk::BlendOp::eAdd,
    .colorWriteMask       = vk::ColorComponentFlagBits::eR |
                            vk::ColorComponentFlagBits::eG |
                            vk::ColorComponentFlagBits::eB |
                            vk::ColorComponentFlagBits::eA
  };

  vk::PipelineColorBlendStateCreateInfo ci_blend{
    .logicOpEnable    = false,
    .attachmentCount  = 1,
    .pAttachments     = &colorAttachment
  };

  vk::PipelineRenderingCreateInfo ci_rendering{
    .colorAttachmentCount = 1,
    .pColorAttachmentFormats = &hlvl_settings.format,
    .depthAttachmentFormat = hlvl_settings.depth_format
  };

  vk::GraphicsPipelineCreateInfo ci_gPipeline{
    .pNext                = &ci_rendering,
    .stageCount           = static_cast<unsigned int>(ci_stages.size()),
    .pStages              = ci_stages.data(),
    .pVertexInputState    = &ci_inputState,
    .pInputAssemblyState  = &ci_assembly,
    .pViewportState       = &ci_viewport,
    .pRasterizationState  = &ci_rasterizer,
    .pMultisampleState    = &ci_multisample,
    .pDepthStencilState   = &ci_depth,
    .pColorBlendState     = &ci_blend,
    .pDynamicState        = &ci_dynamicState,
    .layout               = vk_Layout
  };

  vk_gPipeline = Context::device().createGraphicsPipeline(nullptr, ci_gPipeline);
}

void Material::createTextureDescriptors(MaterialBuilder& materialBuilder) {
  auto [tmp_memory, tmp_images, tmp_views, tmp_samplers] = VulkanFactory::newTextureAllocation(
    materialBuilder.textures
  );

  vk_texMemory = std::move(tmp_memory);
  vk_images = std::move(tmp_images);
  vk_imageViews = std::move(tmp_views);
  vk_samplers = std::move(tmp_samplers);
}

void Material::createStorageDescriptors(MaterialBuilder& materialBuilder) {
  std::vector<vk::BufferCreateInfo> ci_buffers;
  for (auto& [_, resource] : materialBuilder.sResources) {
    ci_buffers.emplace_back(vk::BufferCreateInfo{
      .size         = resource->size,
      .usage        = vk::BufferUsageFlagBits::eTransferSrc,
      .sharingMode  = vk::SharingMode::eExclusive
    });
  }

  auto [stagingMemory, stagingBuffers, offsets, allocationSize] = VulkanFactory::newAllocation(
    ci_buffers, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
  );

  void * memoryMap = stagingMemory.mapMemory(0, allocationSize);

  unsigned int index = 0;
  for (auto& [_, resource] : materialBuilder.sResources) {
    resource->memoryMap = memoryMap;

    resource->offsets.emplace_back(offsets[index]);
    resource->initialize();

    resource->memoryMap = nullptr;
    resource->offsets.clear();
  }

  stagingMemory.unmapMemory();
  memoryMap = nullptr;

  for (auto& ci_buffer : ci_buffers)
    ci_buffer.usage = vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst;

  auto [tmp_sMem, tmp_sBufs, _, __] = VulkanFactory::newAllocation(
    ci_buffers, vk::MemoryPropertyFlagBits::eDeviceLocal
  );
  vk_sMemory = std::move(tmp_sMem);
  vk_sBuffers = std::move(tmp_sBufs);

  auto [pool, cmds] = VulkanFactory::newCommandPool(
    Transfer, stagingBuffers.size(), vk::CommandPoolCreateFlagBits::eTransient
  );

  std::vector<vk::CommandBuffer> submissions;
  for (unsigned int i = 0; i < cmds.size(); ++i) {
    cmds[i].begin({});

    vk::BufferCopy copy{
      .size = materialBuilder.sResources[i % materialBuilder.sResources.size()].second->size
    };

    cmds[i].copyBuffer(stagingBuffers[i], vk_sBuffers[i], copy);
    cmds[i].end();

    submissions.emplace_back(cmds[i]);
  }

  vk::SubmitInfo submitInfo{
    .commandBufferCount = static_cast<unsigned int>(submissions.size()),
    .pCommandBuffers    = submissions.data()
  };

  vk::raii::Fence transferFence = Context::device().createFence({});

  Context::queue(Transfer).submit(submitInfo, transferFence);

  if (Context::device().waitForFences(*transferFence, true, 1000000000ul) != vk::Result::eSuccess)
    throw std::runtime_error("hlvl: hung waiting for storage transfer");
}

void Material::createUniformDescriptors(MaterialBuilder& materialBuilder) {
  std::vector<vk::BufferCreateInfo> ci_buffers;
  for (unsigned int i = 0; i < hlvl_settings.buffer_mode; ++i) {
    for (auto& [_, resource] : materialBuilder.uResources) {
      ci_buffers.emplace_back(vk::BufferCreateInfo{
        .size         = resource->size,
        .usage        = vk::BufferUsageFlagBits::eUniformBuffer,
        .sharingMode  = vk::SharingMode::eExclusive
      });
    }
  }

  auto [tmp_uMem, tmp_uBufs, offsets, allocationSize] = VulkanFactory::newAllocation(
    ci_buffers, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
  );
  vk_uMemory = std::move(tmp_uMem);
  vk_uBuffers = std::move(tmp_uBufs);

  void * memoryMap = vk_uMemory.mapMemory(0, allocationSize);
  unsigned int j = 0;
  for (auto& [_, resource] : materialBuilder.uResources) {
    resource->memoryMap = memoryMap;

    for (unsigned int i = 0; i < hlvl_settings.buffer_mode; ++i)
      resource->offsets.emplace_back(offsets[i * materialBuilder.uResources.size() + j]);
    resource->initialize();

    ++j;
  }
}

void Material::createDescriptorSets(MaterialBuilder& materialBuilder) {
  auto [tmp_pool, tmp_sets] = VulkanFactory::newDescriptorPool(
    vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
    vk_dsLayouts,
    vk_images.size(),
    materialBuilder.sResources.size(),
    materialBuilder.uResources.size()
  );
  vk_descriptorPool = std::move(tmp_pool);
  vk_descriptorSets = std::move(tmp_sets);

  std::vector<vk::WriteDescriptorSet> writes;

  for (unsigned int i = 0; i < hlvl_settings.buffer_mode; ++i) {
    for (unsigned int j = 0; j < vk_images.size(); ++j) {
      vk::DescriptorImageInfo imageInfo{
        .sampler      = vk_samplers[j],
        .imageView    = vk_imageViews[j],
        .imageLayout  = vk::ImageLayout::eShaderReadOnlyOptimal
      };

      writes.emplace_back(vk::WriteDescriptorSet{
        .dstSet           = vk_descriptorSets[0][i],
        .dstBinding       = j,
        .descriptorCount  = 1,
        .descriptorType   = vk::DescriptorType::eCombinedImageSampler,
        .pImageInfo       = &imageInfo
      });
    }

    unsigned int j = 0;
    for (auto& [_, resource] : materialBuilder.sResources) {
      vk::DescriptorBufferInfo bufferInfo{
        .buffer = vk_sBuffers[j],
        .range  = resource->size
      };

      writes.emplace_back(vk::WriteDescriptorSet{
        .dstSet           = vk_descriptorSets[!vk_images.empty()][i],
        .dstBinding       = j,
        .descriptorCount  = 1,
        .descriptorType   = vk::DescriptorType::eStorageBuffer,
        .pBufferInfo      = &bufferInfo
      });

      ++j;
    }

    j = 0;
    for (auto& [_, resource] : materialBuilder.uResources) {
      vk::DescriptorBufferInfo bufferInfo{
        .buffer = vk_uBuffers[i * materialBuilder.uResources.size() + j],
        .range  = resource->size
      };

      writes.emplace_back(vk::WriteDescriptorSet{
        .dstSet           = vk_descriptorSets[(!vk_images.empty()) + (!vk_sBuffers.empty())][i],
        .dstBinding       = j,
        .descriptorCount  = 1,
        .descriptorType   = vk::DescriptorType::eUniformBuffer,
        .pBufferInfo      = &bufferInfo
      });
    }
  }

  Context::device().updateDescriptorSets(writes, nullptr);
}

const Material& Materials::operator [] (std::string tag) const {
  return materialMap.at(tag);
}

Materials& Materials::instance() {
  if (p_materials == nullptr)
    p_materials = new Materials();

  return *p_materials;
}

void Materials::destroy() {
  if (p_materials == nullptr) return;

  delete p_materials;
  p_materials = nullptr;
}

unsigned int Materials::count() const {
  return materialMap.size();
}

void Materials::create(Material::MaterialBuilder& materialBuilder) {
  if (materialMap.find(materialBuilder.tag) != materialMap.end())
    throw std::runtime_error("hlvl: material already exists with tag: " + materialBuilder.tag);

  materialMap.emplace(std::make_pair(materialBuilder.tag, Material(materialBuilder)));
}

} // namespace hlvl