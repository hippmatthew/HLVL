#pragma once

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan_beta.h>

#include <map>
#include <string>

#define hlvl_materials hlvl::Materials::instance()

namespace hlvl {

class Material {
  friend class Materials;

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

      private:
        std::string tag;
        std::map<vk::ShaderStageFlagBits, std::string> shaderMap;
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

  private:
    vk::raii::PipelineLayout vk_gLayout = nullptr;
    vk::raii::Pipeline vk_gPipeline = nullptr;
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