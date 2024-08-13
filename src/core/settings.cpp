#include "src/core/include/settings.hpp"
#include <memory>

namespace pp
{

SettingsManager * SettingsManager::p_manager = nullptr;

General General::default_values()
{
  return General{};
}

void General::add_layers(std::vector<const char *> layers)
{
  unsigned long end = vk_layers.size();
  vk_layers.resize(end + layers.size());
  std::copy(layers.begin(), layers.end(), vk_layers.begin() + end);
}

void General::add_instance_extensions(std::vector<const char *> extensions)
{
  unsigned long end = vk_instance_extensions.size();
  vk_instance_extensions.resize(end + extensions.size());
  std::copy(extensions.begin(), extensions.end(), vk_instance_extensions.begin() + end);
}

void General::add_device_extensions(std::vector<const char *> extensions)
{
  unsigned long end = vk_device_extensions.size();
  vk_device_extensions.resize(end + extensions.size());
  std::copy(extensions.begin(), extensions.end(), vk_device_extensions.begin() + end);
}

Window Window::default_values()
{
  return Window{};
}

float Window::aspect_ratio() const
{
  return size[0] * scale[0] / (size[1] * scale[1]);
}

SettingsManager::SettingsManager()
{
  settingsMap.emplace(std::make_pair(typeid(General).name(), std::make_shared<General>()));
  settingsMap.emplace(std::make_pair(typeid(Window).name(), std::make_shared<Window>()));
}

SettingsManager& SettingsManager::instance()
{
  if (p_manager == nullptr)
    p_manager = new SettingsManager;

  return *p_manager;
}

void SettingsManager::destroy()
{
  if (p_manager == nullptr) return;

  delete p_manager;
  p_manager = nullptr;
}

} // namespace pp