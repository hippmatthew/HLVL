#ifndef physp_core_interface_hpp
#define physp_core_interface_hpp

#include "src/core/include/interface_decl.hpp"

namespace pp
{

template <typename T>
bool Interface<T>::should_close() const
{
  return window.should_close();
}

template <typename T>
void Interface<T>::poll_events() const
{
  window.poll_events();
}

template <typename T>
const vk::raii::SurfaceKHR& Interface<T>::surface() const
{
  return window.surface();
}

template <typename T>
void Interface<T>::create_surface(const vk::raii::Instance& vk_instance)
{
  window.create_surface(vk_instance);
}

} // namespace pp

#endif // physp_core_interface_hpp