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

Material::MaterialBuilder& Material::MaterialBuilder::add_resource(ResourceInfo&& info) {
  resources.emplace_back(info);
  info.type == Uniform ? ++uniformCount : ++storageCount;
  return *this;
}

Material::MaterialBuilder& Material::MaterialBuilder::add_constants(unsigned int s, void * d) {
  constantsSize = s;
  constants = d;
  return *this;
}

Material::Material(MaterialBuilder& materialBuilder) {
  createGraphicsPipeline(materialBuilder);

  if (materialBuilder.storageCount != 0 || materialBuilder.uniformCount != 0) {
    createDescriptors(materialBuilder);
    createDescriptorSets(materialBuilder);
  }

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

  vk::PipelineColorBlendAttachmentState colorAttachment{
    .blendEnable    = false,
    .colorWriteMask = vk::ColorComponentFlagBits::eR |
                      vk::ColorComponentFlagBits::eG |
                      vk::ColorComponentFlagBits::eB |
                      vk::ColorComponentFlagBits::eA
  };

  vk::PipelineColorBlendStateCreateInfo ci_blend{
    .logicOpEnable    = false,
    .attachmentCount  = 1,
    .pAttachments     = &colorAttachment
  };

  std::vector<vk::DescriptorSetLayoutBinding> bindings;
  unsigned int index = 0;
  for (const auto& resource : materialBuilder.resources) {
    bindings.emplace_back(vk::DescriptorSetLayoutBinding{
      .binding          = index++,
      .descriptorType   = resource.type == Uniform ?
                            vk::DescriptorType::eUniformBuffer : vk::DescriptorType::eStorageBuffer,
      .descriptorCount  = 1,
      .stageFlags       = resource.stages
    });
  }

  vk::DescriptorSetLayoutCreateInfo ci_dsLayout{
    .bindingCount = static_cast<unsigned int>(bindings.size()),
    .pBindings    = bindings.data()
  };

  vk_dsLayout = Context::device().createDescriptorSetLayout(ci_dsLayout);

  vk::PushConstantRange range{
    .stageFlags = vk::ShaderStageFlagBits::eAllGraphics,
    .offset     = 0,
    .size       = materialBuilder.constantsSize
  };

  vk::PipelineLayoutCreateInfo ci_layout{
    .setLayoutCount         = 1,
    .pSetLayouts            = &*vk_dsLayout,
    .pushConstantRangeCount = materialBuilder.constantsSize != 0,
    .pPushConstantRanges    = &range
  };

  vk_gLayout = Context::device().createPipelineLayout(ci_layout);

  vk::PipelineRenderingCreateInfo ci_rendering{
    .colorAttachmentCount = 1,
    .pColorAttachmentFormats = &hlvl_settings.format
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
    .pDepthStencilState   = nullptr,
    .pColorBlendState     = &ci_blend,
    .pDynamicState        = &ci_dynamicState,
    .layout               = vk_gLayout
  };

  vk_gPipeline = Context::device().createGraphicsPipeline(nullptr, ci_gPipeline);
}

void Material::createDescriptors(MaterialBuilder& materialBuilder) {
  std::vector<vk::BufferCreateInfo> bufferInfos;
  for (unsigned int i = 0; i < hlvl_settings.buffer_mode; ++i) {
    for (const auto& resource : materialBuilder.resources) {
      bufferInfos.emplace_back(vk::BufferCreateInfo{
        .size = resource.resource->size,
        .usage = static_cast<vk::BufferUsageFlagBits>(resource.type),
        .sharingMode = vk::SharingMode::eExclusive
      });
    }
  }

  auto [tmp_memory, tmp_buffers, offsets, allocationSize] = VulkanFactory::newAllocation(
    bufferInfos,vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
  );
  vk_memory = std::move(tmp_memory);
  vk_buffers = std::move(tmp_buffers);

  void * memoryMap = vk_memory.mapMemory(0, allocationSize);
  unsigned int index = 0;
  for (auto& resource : materialBuilder.resources) {
    resource.resource->memoryMap = memoryMap;

    for (int i = 0; i < hlvl_settings.buffer_mode; ++i)
      resource.resource->offsets.emplace_back(offsets[i * materialBuilder.resources.size() + index]);
    resource.resource->initialize();

    ++index;
  }
}

void Material::createDescriptorSets(MaterialBuilder& materialBuilder) {
  auto [tmp_descriptorPool, tmp_descriptorSets] = VulkanFactory::newDescriptorPool(
    vk_dsLayout,
    vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
    materialBuilder.storageCount,
    materialBuilder.uniformCount
  );
  vk_descriptorPool = std::move(tmp_descriptorPool);
  vk_descriptorSets = std::move(tmp_descriptorSets);

  std::vector<vk::WriteDescriptorSet> writes;
  unsigned int index = 0;
  for (const auto& resource : materialBuilder.resources) {
    for (unsigned int i = 0; i < hlvl_settings.buffer_mode; ++i) {
      vk::DescriptorBufferInfo bufferInfo{
        .buffer = vk_buffers[i * materialBuilder.resources.size() + index],
        .offset = 0,
        .range = resource.resource->size
      };

      writes.emplace_back(vk::WriteDescriptorSet{
        .dstSet           = vk_descriptorSets[i],
        .dstBinding       = index,
        .dstArrayElement  = 0,
        .descriptorCount  = 1,
        .descriptorType   = resource.type == Uniform ?
                              vk::DescriptorType::eUniformBuffer : vk::DescriptorType::eStorageBuffer,
        .pBufferInfo      = &bufferInfo
      });
    }
    ++index;
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