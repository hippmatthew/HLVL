#include "src/core/include/settings.hpp"
#include <memory>

namespace pp
{

SettingsManager * SettingsManager::p_manager = nullptr;

General General::default_values()
{
  return General{};
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