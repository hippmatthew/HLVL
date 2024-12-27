#pragma once

#ifndef hlvl_vulkan_include
#define hlvl_vulkan_include

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan_raii.hpp>

#endif // hlvl_vulkan_include

#include "device.hpp"

namespace hlvl
{

enum Key
{
  a,
  b,
  c,
  d,
  e,
  f,
  g,
  h,
  i,
  j,
  k,
  l,
  m,
  n,
  o,
  p,
  q,
  r,
  s,
  t,
  u,
  v,
  w,
  x,
  y,
  z,
  zero = 0,
  one = 1,
  two = 2,
  three = 3,
  four = 4,
  five = 5,
  six = 6,
  seven = 7,
  eight = 8,
  nine = 9,
  space,
  apostrophe,
  comma,
  minus,
  period,
  slash,
  backslash,
  semicolon,
  equal,
  left_bracket,
  right_bracket,
  grave,
  escape,
  enter,
  tab,
  backspace,
  up,
  down,
  left,
  right,
  caps_lock,
  left_shift,
  left_control,
  left_alt,
  left_super,
  right_shift,
  right_alt,
  right_super,
  unknown
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
    virtual const vk::Image& image(unsigned long) const = 0;
    virtual const vk::raii::ImageView& image_view(unsigned long) const = 0;
    virtual unsigned long next_image_index(const vk::raii::Semaphore&) = 0;

    virtual void create_surface(const vk::raii::Instance&) = 0;
    virtual void load(std::shared_ptr<Device>) = 0;

    const vk::raii::SurfaceKHR& surface() const;

    void add_keybind(Key, std::function<void()>);
    void add_keybind(std::vector<Key>&&, std::function<void()>);

  private:
    void validateBinding(int) const;

  protected:
    void check_format(const vk::raii::PhysicalDevice&) const;
    void check_present_mode(const vk::raii::PhysicalDevice&) const;
    void check_extent(const vk::raii::PhysicalDevice&) const;
    void call_keybind(int) const;

  protected:
    vk::raii::SurfaceKHR vk_surface = nullptr;
    std::map<int, std::function<void()>> key_map;
};

} // namespace hlvl