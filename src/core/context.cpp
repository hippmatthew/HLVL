#include "src/core/include/context.hpp"
#include "src/windows/include/glfw.hpp"

#include <stdexcept>

namespace pp
{

Context::Context()
{
  pp_general_settings.p_context = this;
}

Context::~Context()
{
  p_allocator->wait();

  p_interface.reset();
  p_allocator.reset();
  p_device.reset();
}

Allocator& Context::allocator()
{
  return *p_allocator;
}

ECSController& Context::ecs_controller()
{
  return ecsController;
}

bool Context::should_close() const
{
  return p_interface->should_close();
}

void Context::poll_events() const
{
  p_interface->poll_events();
}

Context& Context::initialize(void * p_next)
{
  if (p_interface == nullptr && pp_general_settings.draw_window)
  {
    pp_general_settings.add_instance_extensions(windows::GLFW::instance_extensions());
    pp_general_settings.add_device_extensions(windows::GLFW::device_extensions());
    set_interface<windows::GLFW>();
  }

  createInstance();

  if (pp_general_settings.draw_window)
  {
    p_interface->create_surface(vk_instance);
    p_device = std::make_shared<Device>(vk_instance, p_interface->surface(), p_next);
  }
  else
    p_device = std::make_shared<Device>(vk_instance, nullptr, p_next);

  p_allocator = std::make_shared<Allocator>(p_device);

  if (pp_general_settings.draw_window)
    p_interface->load(p_device);

  return *this;
}

Context& Context::add_keybind(Key key, std::function<void()> callback)
{
  if (p_interface == nullptr)
    throw std::runtime_error("pp::Context: attempted to bind key callback to a null interface");

  p_interface->add_keybind(key, callback);

  return *this;
}

Context& Context::add_keybind(std::vector<Key>&& keys, std::function<void()> callback)
{
  if (p_interface == nullptr)
    throw std::runtime_error("pp::Context: attempted to bind key callback to a null interface");

  p_interface->add_keybind(std::move(keys), callback);

  return *this;
}

void Context::createInstance()
{
  vk::raii::Context vk_context;

  vk::ApplicationInfo appInfo{
    .pApplicationName   = pp_general_settings.application_name.c_str(),
    .applicationVersion = pp_general_settings.application_version,
    .pEngineName        = "Physics+",
    .engineVersion      = pp_engine_version,
    .apiVersion         = pp_vulkan_version
  };

  for (const auto& ext : vk_context.enumerateInstanceExtensionProperties())
  {
    if (std::string(ext.extensionName) == VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME)
    {
      pp_general_settings.vk_instance_extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
      pp_general_settings.vk_instance_extensions.emplace_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

      pp_general_settings.portability_enabled = true;
      break;
    }
  }

  vk::InstanceCreateInfo ci_instance{
    .pApplicationInfo         = &appInfo,
    .enabledLayerCount        = static_cast<unsigned int>(pp_general_settings.vk_layers.size()),
    .ppEnabledLayerNames      = pp_general_settings.vk_layers.data(),
    .enabledExtensionCount    = static_cast<unsigned int>(pp_general_settings.vk_instance_extensions.size()),
    .ppEnabledExtensionNames  = pp_general_settings.vk_instance_extensions.data()
  };
  if (pp_general_settings.portability_enabled) ci_instance.flags |= vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR;

  vk_instance = vk_context.createInstance(ci_instance);
}

} // namespace pp
