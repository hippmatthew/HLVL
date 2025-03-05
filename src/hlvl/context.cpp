#include "src/hlvl/include/context.hpp"
#include "src/hlvl/include/settings.hpp"

namespace hlvl {

Context::Context() {
  createWindow();
  createInstance();
}

Context::~Context() {
  glfwDestroyWindow(gl_window);
  glfwTerminate();

  Settings::destroy();
}

Context::operator bool() const {
  return !glfwWindowShouldClose(gl_window);
}

void Context::poll_events() {
  glfwPollEvents();
}

void Context::createWindow() {
  glfwInit();

  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  gl_window = glfwCreateWindow(
    hlvl_settings.window_width,
    hlvl_settings.window_height,
    hlvl_settings.window_title.c_str(),
    nullptr,
    nullptr
  );
}

void Context::createInstance() {
  vk::ApplicationInfo appInfo{
    .pApplicationName = hlvl_settings.application_name.c_str(),
    .applicationVersion = hlvl_settings.application_version,
    .pEngineName = "HLVL",
    .engineVersion = hlvl_engine_version,
    .apiVersion = hlvl_vulkan_version
  };

  const char * validationLayer = "VK_LAYER_KHRONOS_validation";

  unsigned int extensionCount = 0;
  const char ** glfwExtensions = glfwGetRequiredInstanceExtensions(&extensionCount);

  std::vector<const char *> extensions(glfwExtensions, glfwExtensions + extensionCount);

  bool portability = false;
  for (auto& extension : vk::enumerateInstanceExtensionProperties()) {
    if (std::string(extension.extensionName) != VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME) continue;

    extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    extensions.emplace_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

    portability = true;
    break;
  }

  vk::InstanceCreateInfo ci_instance{
    .flags = portability ? vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR : vk::InstanceCreateFlags(),
    .pApplicationInfo = &appInfo,
    .enabledLayerCount = 1,
    .ppEnabledLayerNames = &validationLayer,
    .enabledExtensionCount = static_cast<unsigned int>(extensions.size()),
    .ppEnabledExtensionNames = extensions.data()
  };

  vk_instance = vk::raii::Context().createInstance(ci_instance);
}

} // namespace hlvl