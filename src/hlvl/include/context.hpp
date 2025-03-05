#pragma once

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan_beta.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <map>

#define hlvl_vulkan_version VK_MAKE_API_VERSION(1, 4, 304, 0)

#define hlvl_loop_start mainloop: hlvl::Context::poll_events();
#define hlvl_loop_end(c) if(!(c)) goto mainloop;

namespace hlvl
{

enum QueueFamilyType {
  Main = (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT),
  Async = (VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT),
  Compute = VK_QUEUE_COMPUTE_BIT,
  Transfer = VK_QUEUE_TRANSFER_BIT,
  Sparse = VK_QUEUE_SPARSE_BINDING_BIT
};

struct QueueFamily {
  unsigned int index;
  vk::raii::Queue vk_queue;
};

class Context {
  using QueueFamilies = std::map<QueueFamilyType, QueueFamily>;

  public:
    Context();
    Context(Context&) = delete;
    Context(Context&&) = delete;

    ~Context();

    Context& operator=(Context&) = delete;
    Context& operator=(Context&&) = delete;

    explicit operator bool() const;

    static void poll_events();

  #ifdef hlvl_tests

  const GLFWwindow * window() const { return gl_window; }
  const vk::raii::Instance& instance() const { return vk_instance; }
  const vk::raii::SurfaceKHR& surface() const { return vk_surface; }
  const vk::raii::PhysicalDevice& physicalDevice() const { return vk_physicalDevice; }
  const vk::raii::Device& device() const { return vk_device; }
  const QueueFamilies& queueFamilies() const { return qfMap; }

  #endif // hlvl_tests

  private:
    QueueFamilies getQueueFamilies(const vk::raii::PhysicalDevice&) const;
    unsigned int typeIndex(vk::PhysicalDeviceType) const;

    void createWindow();
    bool createInstance();
    void createSurface();
    void chooseGPU();
    void createDevice(bool);

  private:
    GLFWwindow * gl_window = nullptr;

    vk::raii::Instance vk_instance = nullptr;
    vk::raii::SurfaceKHR vk_surface = nullptr;
    vk::raii::PhysicalDevice vk_physicalDevice = nullptr;
    vk::raii::Device vk_device = nullptr;

    QueueFamilies qfMap;

    std::vector<const char *> deviceExtensions = {
      VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
      VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

};

} // namespace hlvl