#include "src/core/include/context.hpp"
#include "src/core/include/settings_decl.hpp"
#include "src/windows/include/glfw.hpp"

namespace pp
{

Context::~Context()
{
  p_interface.reset();
  p_allocator.reset();
  p_device.reset();
}

void Context::initialize(void * p_next)
{
  if (p_interface == nullptr)
  {
    pp_settings_manager.settings<General>().add_instance_extensions(windows::GLFW::instance_extensions());
    set_interface<windows::GLFW>();
  }

  createInstance();

  p_interface->create_surface(vk_instance);

  p_device = std::make_shared<Device>(vk_instance, p_interface->surface(), p_next);
  p_allocator = std::make_shared<Allocator>(p_device);
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
      s_general.vk_instance_extensions.emplace_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

      s_general.portability_enabled = true;
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
  if (s_general.portability_enabled) ci_instance.flags |= vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR;

  vk_instance = vk_context.createInstance(ci_instance);
}

} // namespace pp
