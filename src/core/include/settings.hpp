#ifndef physp_core_settings_hpp
#define physp_core_settings_hpp

#include "src/core/include/settings_decl.hpp"

#include <stdexcept>
#include <type_traits>

namespace pp
{

template <typename T>
T& SettingsManager::settings()
{
  static_assert(std::is_base_of<Settings, T>::value,
    "pp::SettingsManager: attempted to retrieve class that does not inherit from pp::Settings"
  );

  if (!exists<T>())
  {
    throw std::runtime_error(
      "pp::SettingsManager: attempted to retrieve unregistered class"
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
    "pp::SettingsManager: attempted to append class that does not inherit from pp::Settings"
  );

  static_assert(!std::is_same<General, T>::value,
    "pp::SettingsManager: attempted to append default settings class, pp::General"
  );

  if (exists<T>())
  {
    throw std::runtime_error(
      "pp::SettingsManager: attempted to append already registered class"
    );
  }

  settingsMap.emplace(std::make_pair(typeid(T).name(), std::make_shared<T>()));
}

template <typename T>
void SettingsManager::erase()
{
  static_assert(std::is_base_of<Settings, T>::value,
    "pp::SettingsManager: attempted removal of class that does not inherit from pp::Settings"
  );

  static_assert(!std::is_same<General, T>::value,
    "pp::SettingsManager: attempted removal of default settings class, pp::General"
  );

  if (!exists<T>())
  {
    throw std::runtime_error(
      "pp::SettingsManager: attempted to erase unregistered class"
    );
  }

  settingsMap.erase(typeid(T).name());
}

} // namespace pp

#endif // physp_core_settings_hpp
