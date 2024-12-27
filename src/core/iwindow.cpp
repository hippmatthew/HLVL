#include "include/iwindow.hpp"
#include "include/settings_decl.hpp"

#include <limits>
#include <stdexcept>

namespace hlvl
{

const vk::raii::SurfaceKHR& IWindow::surface() const
{
  return vk_surface;
}

void IWindow::add_keybind(Key key, std::function<void()> callback)
{
  validateBinding(key);

  key_map.emplace(std::make_pair(key, callback));
}

void IWindow::add_keybind(std::vector<Key>&& keys, std::function<void()> callback)
{
  int bits = (1 << 31);
  for (Key& key : keys)
    bits |= key;

  validateBinding(bits);

  key_map.emplace(std::make_pair(bits, callback));
}

void IWindow::validateBinding(int bits) const
{
  if (key_map.find(bits) != key_map.end())
    throw std::runtime_error("hlvl::IWindow: attempted to submit callback to a keybind that alread has one");
}

void IWindow::call_keybind(int bits) const
{
  if (key_map.find(bits) != key_map.end())
    key_map.at(bits)();
}

void IWindow::check_format(const vk::raii::PhysicalDevice& vk_physicalDevice) const
{
  auto formats = vk_physicalDevice.getSurfaceFormatsKHR(*vk_surface);

  for (const auto& format : formats)
  {
    if (format.format == hlvl_window_settings.format && format.colorSpace == hlvl_window_settings.color_space)
      return;
  }

  hlvl_window_settings.format = formats[0].format;
  hlvl_window_settings.color_space = formats[0].colorSpace;
}

void IWindow::check_present_mode(const vk::raii::PhysicalDevice& vk_physicalDevice) const
{
  for (const auto& mode : vk_physicalDevice.getSurfacePresentModesKHR(*vk_surface))
    if (mode == hlvl_window_settings.present_mode) return;

  hlvl_window_settings.present_mode = vk::PresentModeKHR::eFifo;
}

void IWindow::check_extent(const vk::raii::PhysicalDevice& vk_physicalDevice) const
{
  if (hlvl_window_settings.extent().width != std::numeric_limits<unsigned int>::max() &&
        hlvl_window_settings.extent().height != std::numeric_limits<unsigned int>::max())
  {
    return;
  }

  auto capabilities = vk_physicalDevice.getSurfaceCapabilitiesKHR(*vk_surface);

  std::array<unsigned int, 4> range = {
    capabilities.minImageExtent.width,
    capabilities.maxImageExtent.width,
    capabilities.minImageExtent.height,
    capabilities.maxImageExtent.height
  };

  for (unsigned int i = 0; i < 2; ++i)
  {
    if (hlvl_window_settings.size[i] * hlvl_window_settings.scale[i] < range[2 * i])
    {
      hlvl_window_settings.size[i] = range[2 * i] / hlvl_window_settings.scale[i];
    }
    else if (hlvl_window_settings.size[i] * hlvl_window_settings.scale[i] > range[2 * i + 1])
    {
      hlvl_window_settings.size[i] = range[2 * i + 1] / hlvl_window_settings.scale[i];
    }
  }
}

} // namespace hlvl