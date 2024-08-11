#include "src/core/include/context.hpp"
#include "src/core/include/settings_decl.hpp"
#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_handles.hpp"

namespace pp
{

Context::Context()
{
  createInstance();
}

void Context::createInstance()
{
  vk::raii::Context vk_context;
  auto& s_general = pp_settings_manager.settings<General>();

  vk::ApplicationInfo appInfo{
    .pApplicationName   = s_general.application_name.c_str(),
    .applicationVersion = s_general.application_version,
    .pEngineName        = "Physics+",
    .engineVersion      = pp_engine_version,
    .apiVersion         = pp_vulkan_version
  };

  for (const auto& ext : vk_context.enumerateInstanceExtensionProperties())
  {
    if (std::string(ext.extensionName) == VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME)
    {
      s_general.vk_instance_extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
      s_general.portability = true;
      break;
    }
  }

  vk::InstanceCreateInfo ci_instance{
    .pApplicationInfo         = &appInfo,
    .enabledLayerCount        = static_cast<unsigned int>(s_general.vk_layers.size()),
    .ppEnabledLayerNames      = s_general.vk_layers.data(),
    .enabledExtensionCount    = static_cast<unsigned int>(s_general.vk_instance_extensions.size()),
    .ppEnabledExtensionNames  = s_general.vk_instance_extensions.data()
  };

  vk_instance = vk_context.createInstance(ci_instance);
}

} // namespace pp