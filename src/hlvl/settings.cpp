#include "src/hlvl/include/settings.hpp"

namespace hlvl {

Settings * Settings::p_settings = nullptr;

Settings& Settings::instance() {
  if (p_settings == nullptr)
    p_settings = new Settings();

  return *p_settings;
}

void Settings::destroy() {
  if (p_settings == nullptr) return;

  delete p_settings;
  p_settings = nullptr;
}

void Settings::reset_to_default() {
  destroy();
  p_settings = new Settings();
}

} // namespace hlvl