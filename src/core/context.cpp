#include "src/core/include/context.hpp"
#include "src/core/include/materials.hpp"
#include "src/core/include/settings.hpp"

#include <stdexcept>

namespace hlvl {

Context::Context() {
  if (p_context != nullptr)
    throw std::runtime_error("hlvl: tried to initialize more than one context");

  p_context = this;

  createWindow();
  bool portability = createInstance();
  createSurface();
  chooseGPU();
  createDevice(portability);
}

Context::~Context() {
  glfwDestroyWindow(gl_window);
  glfwTerminate();

  Materials::destroy();
  Settings::destroy();
}

void Context::poll_events() {
  glfwPollEvents();
}

bool Context::should_close() {
  return glfwWindowShouldClose(p_context->gl_window);
}

const vk::raii::SurfaceKHR& Context::surface() {
  return p_context->vk_surface;
}

const vk::raii::PhysicalDevice& Context::physicalDevice() {
  return p_context->vk_physicalDevice;
}

const vk::raii::Device& Context::device() {
  return p_context->vk_device;
}

Context::QueueFamilies Context::getQueueFamilies(const vk::raii::PhysicalDevice& gpu) const {
  QueueFamilies families;

  unsigned int index = 0;
  for (auto& property : gpu.getQueueFamilyProperties()) {
    if ((property.queueFlags & vk::QueueFlagBits::eSparseBinding) && families.find(Sparse) == families.end()) {
      families.emplace(std::make_pair(Sparse, QueueFamily{ index++, nullptr }));
      continue;
    }

    if ((property.queueFlags & static_cast<vk::QueueFlagBits>(Main))
          && families.find(Main) == families.end()
          && gpu.getSurfaceSupportKHR(index, vk_surface)
    ) {
      families.emplace(std::make_pair(Main, QueueFamily{ index++, nullptr }));
      continue;
    }

    if ((property.queueFlags & static_cast<vk::QueueFlagBits>(Async))
          && families.find(Async) == families.end()
    ) {
      families.emplace(std::make_pair(Async, QueueFamily{ index++, nullptr }));
      continue;
    }

    if ((property.queueFlags & static_cast<vk::QueueFlagBits>(Compute))
          && families.find(Compute) == families.end()
    ) {
      families.emplace(std::make_pair(Compute, QueueFamily{ index++, nullptr }));
      continue;
    }

    if ((property.queueFlags & static_cast<vk::QueueFlagBits>(Transfer))
          && families.find(Transfer) == families.end()
    ) {
      families.emplace(std::make_pair(Transfer, QueueFamily{ index++, nullptr }));
      continue;
    }

    ++index;
  }

  return families;
}

unsigned int Context::typeIndex(vk::PhysicalDeviceType type) const {
  switch (type) {
    case vk::PhysicalDeviceType::eDiscreteGpu:
      return 0;
    case vk::PhysicalDeviceType::eIntegratedGpu:
      return 1;
    case vk::PhysicalDeviceType::eVirtualGpu:
      return 2;
    default:
      return -1;
  }
}

void Context::createWindow() {
  if (glfwInit() != GLFW_TRUE)
    throw std::runtime_error("hlvl: failed to initialize glfw");

  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  gl_window = glfwCreateWindow(
    hlvl_settings.window_width,
    hlvl_settings.window_height,
    hlvl_settings.window_title.c_str(),
    nullptr,
    nullptr
  );

  if (gl_window == nullptr)
    throw std::runtime_error("hlvl: failed to create window");
}

bool Context::createInstance() {
  vk::ApplicationInfo appInfo{
    .pApplicationName   = hlvl_settings.application_name.c_str(),
    .applicationVersion = hlvl_settings.application_version,
    .pEngineName        = "HLVL",
    .engineVersion      = hlvl_engine_version,
    .apiVersion         = hlvl_vulkan_version
  };

  const char * validationLayer = "VK_LAYER_KHRONOS_validation";

  unsigned int extensionCount = 0;
  const char ** glfwExtensions = glfwGetRequiredInstanceExtensions(&extensionCount);
  if (glfwExtensions == nullptr)
    throw std::runtime_error("hlvl: failed to get glfw extensions");


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
    .pApplicationInfo         = &appInfo,
    .enabledLayerCount        = 1,
    .ppEnabledLayerNames      = &validationLayer,
    .enabledExtensionCount    = static_cast<unsigned int>(extensions.size()),
    .ppEnabledExtensionNames  = extensions.data()
  };

  vk_instance = vk::raii::Context().createInstance(ci_instance);

  return portability;
}

void Context::createSurface() {
  VkSurfaceKHR surface;
  if (glfwCreateWindowSurface(*vk_instance, gl_window, nullptr, &surface) != VK_SUCCESS)
    throw std::runtime_error("hlvl: failed to create window surface");

  vk_surface = vk::raii::SurfaceKHR(vk_instance, surface);
  if (vk_surface == nullptr)
    throw std::runtime_error("hlvl: failed to create window surface");
}

void Context::chooseGPU() {
  const bool comparisonMap[3][3] = {
    { false, false, false },
    { true,  false, false },
    { true,  true,  false }
  };

  vk::PhysicalDeviceType prevType = vk::PhysicalDeviceType::eOther;

  for (const auto& gpu : vk_instance.enumeratePhysicalDevices()) {
    auto properties = gpu.getProperties();

    switch(properties.deviceType) {
      case vk::PhysicalDeviceType::eDiscreteGpu:
      case vk::PhysicalDeviceType::eIntegratedGpu:
      case vk::PhysicalDeviceType::eVirtualGpu:
        break;
      default:
        continue;
    }

    QueueFamilies families = getQueueFamilies(gpu);
    if (families.find(Main) == families.end()) continue;

    if (gpu.getSurfaceFormatsKHR(vk_surface).empty()) continue;
    if (gpu.getSurfacePresentModesKHR(vk_surface).empty()) continue;

    std::map<std::string, char> extMap;
    for (const auto& ext : gpu.enumerateDeviceExtensionProperties())
      extMap.emplace(std::pair(std::string(ext.extensionName), 0));

    bool supported = true;
    for (const char * ext : deviceExtensions) {
      if (extMap.find(std::string(ext)) == extMap.end()) {
        supported = false;
        break;
      }
    }
    if (!supported) continue;

    if (prevType == vk::PhysicalDeviceType::eOther
          || comparisonMap[typeIndex(prevType)][typeIndex(properties.deviceType)]
    ) {
      vk_physicalDevice = gpu;
      prevType = properties.deviceType;
      qfMap = std::move(families);
    }
  }

  if (vk_physicalDevice == nullptr)
    throw std::runtime_error("hlvl: failed to find suitable gpu");
}

void Context::createDevice(bool portability) {
  std::vector<vk::DeviceQueueCreateInfo> ci_queues;
  float priority = 1.0f;
  for (auto& [_, family] : qfMap) {
    ci_queues.emplace_back(vk::DeviceQueueCreateInfo{
      .queueFamilyIndex = family.index,
      .queueCount       = 1,
      .pQueuePriorities = &priority
    });
  }

  if (portability) {
    std::map<std::string, char> extMap;
    for (const auto& ext : vk_physicalDevice.enumerateDeviceExtensionProperties())
      extMap.emplace(std::make_pair(std::string(ext.extensionName), 0));

    if (extMap.find(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME) != extMap.end())
      deviceExtensions.emplace_back(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
  }

  vk::PhysicalDeviceFeatures features;

  vk::PhysicalDeviceDynamicRenderingFeaturesKHR dynRender{
    .dynamicRendering = vk::True
  };

  vk::DeviceCreateInfo ci_device{
    .pNext                    = &dynRender,
    .queueCreateInfoCount     = static_cast<unsigned int>(ci_queues.size()),
    .pQueueCreateInfos        = ci_queues.data(),
    .enabledExtensionCount    = static_cast<unsigned int>(deviceExtensions.size()),
    .ppEnabledExtensionNames  = deviceExtensions.data(),
    .pEnabledFeatures         = &features
  };

  vk_device = vk_physicalDevice.createDevice(ci_device);

  for (auto& [_, family] : qfMap)
    family.vk_queue = vk_device.getQueue(family.index, 0);
}

} // namespace hlvl