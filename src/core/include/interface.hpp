#pragma once

#include "interface_decl.hpp"

namespace hlvl
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
const vk::Image& Interface<T>::image(unsigned long index) const
{
  return window.image(index);
}

template <typename T>
const vk::raii::ImageView& Interface<T>::image_view(unsigned long index) const
{
  return window.image_view(index);
}

template <typename T>
unsigned long Interface<T>::next_image_index(const vk::raii::Semaphore& semaphore)
{
  return window.next_image_index(semaphore);
}

template <typename T>
void Interface<T>::add_keybind(Key key, std::function<void()> callback)
{
  window.add_keybind(key, callback);
}

template <typename T>
void Interface<T>::add_keybind(std::vector<Key>&& keys, std::function<void()> callback)
{
  window.add_keybind(std::move(keys), callback);
}

template <typename T>
void Interface<T>::create_surface(const vk::raii::Instance& vk_instance)
{
  window.create_surface(vk_instance);
}

template <typename T>
void Interface<T>::load(std::shared_ptr<Device> p_device)
{
  window.load(p_device);
}

} // namespace hlvl