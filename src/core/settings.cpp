#include "src/core/include/settings.hpp"
#include <memory>

namespace pp
{

SettingsManager * SettingsManager::p_manager = nullptr;

GeneralSettings GeneralSettings::default_values()
{
  return GeneralSettings{};
}

void GeneralSettings::add_layers(std::vector<const char *>&& layers)
{
  vk_layers.insert(vk_layers.end(), layers.begin(), layers.end());
}

void GeneralSettings::add_instance_extensions(std::vector<const char *>&& extensions)
{
  vk_instance_extensions.insert(vk_instance_extensions.end(), extensions.begin(), extensions.end());
}

void GeneralSettings::add_device_extensions(std::vector<const char *>&& extensions)
{
  vk_device_extensions.insert(vk_device_extensions.end(), extensions.begin(), extensions.end());
}

WindowSettings WindowSettings::default_values()
{
  return WindowSettings{};
}

float WindowSettings::aspect_ratio() const
{
  return (size[0] * scale[0]) / (size[1] * scale[1]);
}

vk::Extent2D WindowSettings::extent() const
{
  return vk::Extent2D{
    .width = static_cast<unsigned int>(size[0] * scale[0]),
    .height = static_cast<unsigned int>(size[1] * scale[1])
  };
}

SettingsManager::SettingsManager()
{
  settingsMap.emplace(std::make_pair(typeid(GeneralSettings).name(), std::make_shared<GeneralSettings>()));
  settingsMap.emplace(std::make_pair(typeid(WindowSettings).name(), std::make_shared<WindowSettings>()));
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