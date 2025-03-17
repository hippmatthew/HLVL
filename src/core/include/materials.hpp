#pragma once

#include "src/core/include/resource.hpp"

#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan_beta.h>

#include <map>
#include <string>

#define hlvl_materials hlvl::Materials::instance()

namespace hlvl {

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
        MaterialBuilder& add_storage(vk::ShaderStageFlagBits, ResourceProxy *);
        MaterialBuilder& add_uniform(vk::ShaderStageFlagBits, ResourceProxy *);
        MaterialBuilder& add_constants(unsigned int, void *);
        MaterialBuilder& add_texture(std::string);

      private:
        std::string tag;

        std::map<vk::ShaderStageFlagBits, std::string> shaderMap;

        std::vector<std::string> textures;
        std::vector<std::pair<vk::ShaderStageFlagBits, ResourceProxy *>> sResources;
        std::vector<std::pair<vk::ShaderStageFlagBits, ResourceProxy *>> uResources;

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

      const vk::raii::PipelineLayout& get_layout() const { return vk_Layout; }
      const vk::raii::Pipeline& get_gPipeline() const { return vk_gPipeline; }
      const std::vector<vk::raii::DescriptorSetLayout>& get_dsLayouts() const { return vk_dsLayouts; }
      const vk::raii::DescriptorPool& get_dsPool() const { return vk_descriptorPool; }
      const std::vector<vk::raii::DescriptorSets>& get_sets() const { return vk_descriptorSets; }
      const vk::raii::DeviceMemory& get_texMem() const { return vk_texMemory; }
      const std::vector<vk::raii::Image>& get_images() const { return vk_images; }
      const std::vector<vk::raii::ImageView>& get_views() const { return vk_imageViews; }
      const std::vector<vk::raii::Sampler>& get_samplers() const { return vk_samplers; }
      const vk::raii::DeviceMemory& get_sMem() const { return vk_sMemory; }
      const std::vector<vk::raii::Buffer>& get_sBufs() const { return vk_sBuffers; }
      const vk::raii::DeviceMemory& get_uMem() const { return vk_uMemory; }
      const std::vector<vk::raii::Buffer>& get_uBufs() const { return vk_uBuffers; }
      const unsigned int& get_constantsSize() const { return constantsSize; }
      const void * get_constants() const { return constants; }

    #endif

  private:
    std::vector<char> read(std::string) const;
    std::pair<
      std::vector<vk::raii::ShaderModule>,
      std::vector<vk::PipelineShaderStageCreateInfo>
    > processShaders(MaterialBuilder&) const;

    void createLayout(MaterialBuilder&);
    void createGraphicsPipeline(MaterialBuilder&);
    void createTextureDescriptors(MaterialBuilder&);
    void createStorageDescriptors(MaterialBuilder&);
    void createUniformDescriptors(MaterialBuilder&);
    void createDescriptorSets(MaterialBuilder&);

  private:
    vk::raii::PipelineLayout vk_Layout = nullptr;
    vk::raii::Pipeline vk_gPipeline = nullptr;

    std::vector<vk::raii::DescriptorSetLayout> vk_dsLayouts;
    vk::raii::DescriptorPool vk_descriptorPool = nullptr;
    std::vector<vk::raii::DescriptorSets> vk_descriptorSets;

    vk::raii::DeviceMemory vk_texMemory = nullptr;
    std::vector<vk::raii::Image> vk_images;
    std::vector<vk::raii::ImageView> vk_imageViews;
    std::vector<vk::raii::Sampler> vk_samplers;

    vk::raii::DeviceMemory vk_sMemory = nullptr;
    std::vector<vk::raii::Buffer> vk_sBuffers;

    vk::raii::DeviceMemory vk_uMemory = nullptr;
    std::vector<vk::raii::Buffer> vk_uBuffers;

    unsigned int constantsSize = 0;
    void * constants = nullptr;
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