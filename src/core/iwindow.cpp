#include "src/core/include/iwindow.hpp"
#include "src/core/include/settings_decl.hpp"
#include "vulkan/vulkan_enums.hpp"
#include <limits>

namespace pp
{

const vk::raii::SurfaceKHR& IWindow::surface() const
{
  return vk_surface;
}

void IWindow::check_format(const vk::raii::PhysicalDevice& vk_physicalDevice) const
{
  auto& s_window = pp_window_settings;
  auto formats = vk_physicalDevice.getSurfaceFormatsKHR(*vk_surface);

  for (const auto& format : formats)
  {
    if (format.format == s_window.format && format.colorSpace == s_window.color_space)
      return;
  }

  s_window.format = formats[0].format;
  s_window.color_space = formats[0].colorSpace;
}

void IWindow::check_present_mode(const vk::raii::PhysicalDevice& vk_physicalDevice) const
{
  auto& s_window = pp_window_settings;

  for (const auto& mode : vk_physicalDevice.getSurfacePresentModesKHR(*vk_surface))
    if (mode == s_window.present_mode) return;

  s_window.present_mode = vk::PresentModeKHR::eFifo;
}

void IWindow::check_extent(const vk::raii::PhysicalDevice& vk_physicalDevice) const
{
  auto& s_window = pp_window_settings;

  if (s_window.extent().width != std::numeric_limits<unsigned int>::max() &&
        s_window.extent().height != std::numeric_limits<unsigned int>::max())
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
    if (s_window.size[i] * s_window.scale[i] < range[2 * i])
      s_window.size[i] = range[2 * i] / s_window.scale[i];
    else if (s_window.size[i] * s_window.scale[i] > range[2 * i + 1])
      s_window.size[i] = range[2 * i + 1] / s_window.scale[i];
  }
}

} // namespace pp