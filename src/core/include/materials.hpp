#pragma once

#include "src/core/include/resource.hpp"

#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan_beta.h>

#include <map>
#include <string>

#define hlvl_materials hlvl::Materials::instance()

namespace hlvl {

enum BufferType {
  Uniform = static_cast<unsigned int>(vk::BufferUsageFlagBits::eUniformBuffer),
  Storage = static_cast<unsigned int>(vk::BufferUsageFlagBits::eStorageBuffer)
};

struct ResourceInfo {
  BufferType type = Uniform;
  vk::ShaderStageFlags stages;
  ResourceProxy * resource;
};

class Material {
  friend class Materials;
  friend class Renderer;

  private:
    class MaterialBuilder {
      friend class Material;
      friend class Materials;

      public:
        MaterialBuilder(std::string);
        MaterialBuilder(MaterialBuilder&) = delete;
        MaterialBuilder(MaterialBuilder&&) = delete;

        ~MaterialBuilder() = default;

        MaterialBuilder& operator = (MaterialBuilder&) = delete;
        MaterialBuilder& operator = (MaterialBuilder&&) = delete;

        MaterialBuilder& add_shader(vk::ShaderStageFlagBits, std::string);
        MaterialBuilder& add_resource(ResourceInfo&&);
        MaterialBuilder& add_constants(unsigned int, void *);

      private:
        std::string tag;

        std::map<vk::ShaderStageFlagBits, std::string> shaderMap;

        std::vector<ResourceInfo> resources;
        unsigned int storageCount = 0;
        unsigned int uniformCount = 0;

        unsigned int constantsSize = 0;
        void * constants = nullptr;
    };

  public:
    Material() = delete;
    Material(Material&) = delete;
    Material(Material&&) = default;
    Material(MaterialBuilder&);

    ~Material() = default;

    Material& operator = (Material&) = delete;
    Material& operator = (Material&&) = default;

    static MaterialBuilder builder(std::string);

    #ifdef hlvl_tests

      const vk::raii::PipelineLayout& get_gLayout() const { return vk_gLayout; }
      const vk::raii::Pipeline& get_gPipeline() const { return vk_gPipeline; }

    #endif // hlvl_tests

  private:
    std::vector<char> read(std::string) const;
    std::pair<
      std::vector<vk::raii::ShaderModule>,
      std::vector<vk::PipelineShaderStageCreateInfo>
    > processShaders(MaterialBuilder&) const;

    void createGraphicsPipeline(MaterialBuilder&);
    void createDescriptors(MaterialBuilder&);
    void createDescriptorSets(MaterialBuilder&);

  private:
    vk::raii::PipelineLayout vk_gLayout = nullptr;
    vk::raii::Pipeline vk_gPipeline = nullptr;

    vk::raii::DeviceMemory vk_memory = nullptr;
    std::vector<vk::raii::Buffer> vk_buffers;

    unsigned int constantsSize = 0;
    void * constants = nullptr;

    vk::raii::DescriptorSetLayout vk_dsLayout = nullptr;
    vk::raii::DescriptorPool vk_descriptorPool = nullptr;
    vk::raii::DescriptorSets vk_descriptorSets = nullptr;
};

class Materials {
  public:
    Materials(Materials&) = delete;
    Materials(Materials&&) = delete;

    Materials& operator = (Materials&) = delete;
    Materials& operator = (Materials&&) = delete;

    const Material& operator[] (std::string) const;

    static Materials& instance();
    static void destroy();

    unsigned int count() const;
    void create(Material::MaterialBuilder&);

  private:
    Materials() = default;
    ~Materials() = default;

  private:
    static Materials * p_materials;
    std::map<std::string, Material> materialMap;
};

} // namespace hlvl