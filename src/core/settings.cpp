#include "src/core/include/settings.hpp"
#include <memory>

namespace pp
{

SettingsManager * SettingsManager::p_manager = nullptr;

General General::default_values()
{
  return General{};
}

SettingsManager::SettingsManager()
{
  settingsMap.emplace(std::make_pair(typeid(General).name(), std::make_shared<General>()));
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