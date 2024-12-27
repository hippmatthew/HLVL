#pragma once

#include "settings_decl.hpp"

#include <stdexcept>
#include <type_traits>

namespace hlvl
{

template <typename T>
T& SettingsManager::settings()
{
  static_assert(std::is_base_of<Settings, T>::value,
    "hlvl::SettingsManager: attempted to retrieve class that does not inherit from hlvl::Settings"
  );

  if (!exists<T>())
  {
    throw std::runtime_error(
      "hlvl::SettingsManager: attempted to retrieve unregistered class"
    );
  }

  return *(std::static_pointer_cast<T>(settingsMap.at(typeid(T).name())));
}

template <typename... Tps>
void SettingsManager::add_settings()
{
  ( append<Tps>(), ... );
}

template <typename... Tps>
void SettingsManager::remove_settings()
{
  ( erase<Tps>(), ... );
}

template <typename T>
bool SettingsManager::exists() const
{
  return settingsMap.find(typeid(T).name()) != settingsMap.end();
}

template <typename T>
void SettingsManager::append()
{
  static_assert(std::is_base_of<Settings, T>::value,
    "hlvl::SettingsManager: attempted to append class that does not inherit from hlvl::Settings"
  );

  static_assert(!std::is_same<GeneralSettings, T>::value,
    "hlvl::SettingsManager: attempted to append default settings class hlvl::GeneralSettings"
  );

  static_assert(!std::is_same<WindowSettings, T>::value,
    "hlvl:SettingsManager: attempted to append default settings class hlvl::WindowSettings"
  );

  if (exists<T>())
  {
    throw std::runtime_error(
      "hlvl::SettingsManager: attempted to append already registered class"
    );
  }

  settingsMap.emplace(std::make_pair(typeid(T).name(), std::make_shared<T>()));
}

template <typename T>
void SettingsManager::erase()
{
  static_assert(std::is_base_of<Settings, T>::value,
    "hlvl::SettingsManager: attempted removal of class that does not inherit from hlvl::Settings"
  );

  static_assert(!std::is_same<GeneralSettings, T>::value,
    "hlvl::SettingsManager: attempted removal of default settings class hlvl::General"
  );

  static_assert(!std::is_same<WindowSettings, T>::value,
    "hlvl:SettingsManager: attempted removeal of default settings class hlvl::WindowSettings"
  );

  if (!exists<T>())
  {
    throw std::runtime_error(
      "hlvl::SettingsManager: attempted to erase unregistered class"
    );
  }

  settingsMap.erase(typeid(T).name());
}

} // namespace hlvl
