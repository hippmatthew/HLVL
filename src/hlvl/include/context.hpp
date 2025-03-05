#pragma once

#ifndef hlvl_vulkan_include
#define hlvl_vulkan_include

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan_raii.hpp>

#endif

#ifndef hlvl_glfw_include
#define hlvl_glfw_include

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#endif

#define hlvl_vulkan_version VK_MAKE_API_VERSION(1, 4, 304, 0)

#define hlvl_loop_start mainloop: hlvl::Context::poll_events();
#define hlvl_loop_end(c) if(!(c)) goto mainloop;

namespace hlvl
{

class Context {
  public:
    Context();
    Context(Context&) = delete;
    Context(Context&&) = delete;

    ~Context();

    Context& operator=(Context&) = delete;
    Context& operator=(Context&&) = delete;

    explicit operator bool() const;

    static void poll_events();

  private:
    void createWindow();
    void createInstance();

  private:
    GLFWwindow * gl_window = nullptr;
    vk::raii::Instance vk_instance = nullptr;
};

} // namespace hlvl