#ifndef physp_core_iwindow_decl_hpp
#define physp_core_iwindow_decl_hpp

#ifndef physp_vulkan_include
#define physp_vulkan_include

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan_raii.hpp>

#endif // physp_vulkan_include

namespace pp
{

class KeyCallback
{
  public:
    KeyCallback() = default;
    KeyCallback(const KeyCallback&) = default;
    KeyCallback(KeyCallback&&) = default;

    virtual ~KeyCallback() = default;

    KeyCallback& operator = (const KeyCallback&) = default;
    KeyCallback& operator = (KeyCallback&&) = default;

    virtual void operator () () = 0;
};

class IWindow
{
  public:
    IWindow() = default;
    IWindow(IWindow&) = delete;
    IWindow(IWindow&&) = delete;

    virtual ~IWindow() = default;

    IWindow& operator = (IWindow&) = delete;
    IWindow& operator = (IWindow&&) = delete;

    virtual bool should_close() const = 0;
    virtual void poll_events() const = 0;
    virtual void create_surface(const vk::raii::Instance&) = 0;

    const vk::raii::SurfaceKHR& surface() const;

    // template <typename T>
    // void add_binding(Key);

    // template <typename T>
    // void add_binding(std::vector<Key>);

  protected:
    // void create_swapchain(const vk::raii::Device&, const vk::raii::PhysicalDevice&);

  protected:
    // std::map<std::string, std::shared_ptr<KeyCallback>> keyMap;
    vk::raii::SurfaceKHR vk_surface = nullptr;
};

} // namespace pp

#endif // physp_core_iwindow_decl_hpp