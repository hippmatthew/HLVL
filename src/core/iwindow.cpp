#include "src/core/include/iwindow.hpp"

namespace pp
{

const vk::raii::SurfaceKHR& IWindow::surface() const
{
  return vk_surface;
}

} // namespace pp