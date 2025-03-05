#pragma once

#include <string>

#define hlvl_settings hlvl::Settings::instance()

#define hlvl_make_version(major, minor, patch) (unsigned int)( \
  ( (major) << 24 ) | \
  ( (minor) << 16 ) | \
  (patch) \
)

#define hlvl_engine_version hlvl_make_version(0, 67, 0)

namespace hlvl {

class Settings {
  public:
    Settings(Settings&) = delete;
    Settings(Settings&&) = delete;

    Settings& operator = (const Settings&);
    Settings& operator = (Settings&&) = delete;

    static Settings& instance();
    static void destroy();
    static void reset_to_default();

  private:
    Settings() = default;
    ~Settings() = default;

  public:
    unsigned int window_width = 1280;
    unsigned int window_height = 720;
    std::string window_title = "HLVL";

    std::string application_name = "HLVL Application";
    unsigned int application_version = hlvl_make_version(1, 0, 0);

  private:
    static Settings * p_settings;
};

} // namespace hlvl