#ifndef physp_core_settings_hpp
#define physp_core_settings_hpp

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan_raii.hpp>

#include <map>
#include <memory>
#include <string>

#define pp_make_version(major, minor, patch) (  \
  ((major) << 16) | \
  ((minor) << 8)  | \
  ((patch))         \
)
#define pp_vulkan_version VK_MAKE_API_VERSION(VK_API_VERSION_1_3, 1, 3, 290)
#define pp_engine_version pp_make_version(0, 0, 0)
#define pp_settings_manager pp::SettingsManager::instance()

namespace pp
{

class Settings
{
  public:
    Settings() = default;
    Settings(const Settings&) = default;
    Settings(Settings&&) = default;

    virtual ~Settings() = default;

    virtual Settings& operator = (const Settings&) = default;
    virtual Settings& operator = (Settings&&) = default;
};

class General : public Settings
{
  public:
    static General default_values();

  public:
    std::string application_name = "PP Application";
    unsigned int application_version = pp_make_version(1, 0, 0);
};

class SettingsManager
{
  public:
    SettingsManager(SettingsManager&) = delete;
    SettingsManager(SettingsManager&&) = delete;

    SettingsManager& operator = (SettingsManager&) = delete;
    SettingsManager& operator = (SettingsManager&&) = delete;

    static SettingsManager& instance();
    static void destroy();

    void reset_to_default();

    template <typename T>
    T& settings();

    template <typename... Tps>
    void add_settings();

    template <typename... Tps>
    void remove_settings();

  private:
    SettingsManager();
    ~SettingsManager() = default;

    template <typename T>
    bool exists() const;

    template <typename T>
    void append();

    template <typename T>
    void erase();

  private:
    static SettingsManager * p_manager;
    std::map<const char *, std::shared_ptr<Settings>> settingsMap;
};

} // namespace pp

#endif // physp_core_settings_hpp