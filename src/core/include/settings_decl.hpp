#ifndef physp_core_settings_decl_hpp
#define physp_core_settings_decl_hpp

#ifndef physp_vulkan_include
#define physp_vulkan_include

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan_raii.hpp>

#endif // physp_vulkan_include

#include <map>
#include <memory>
#include <string>

#define pp_make_version(major, minor, patch) (  \
  ((major) << 16) | \
  ((minor) << 8)  | \
  ((patch))         \
)
#define pp_vulkan_version   VK_MAKE_API_VERSION(0, 1, 3, 290)
#define pp_engine_version   pp_make_version(0, 45, 2)

#define pp_settings_manager pp::SettingsManager::instance()
#define pp_general_settings pp_settings_manager.settings<pp::GeneralSettings>()
#define pp_window_settings  pp_settings_manager.settings<pp::WindowSettings>()

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

class Context;

class GeneralSettings : public Settings
{
  public:
    GeneralSettings() = default;
    GeneralSettings(const GeneralSettings&) = default;
    GeneralSettings(GeneralSettings&&) = default;

    ~GeneralSettings() = default;

    GeneralSettings& operator = (const GeneralSettings&) = default;
    GeneralSettings& operator = (GeneralSettings&&) = default;

    static GeneralSettings default_values();

    void add_layers(std::vector<const char *>&&);
    void add_instance_extensions(std::vector<const char *>&&);
    void add_device_extensions(std::vector<const char *>&&);

  public:
    Context * p_context = nullptr;
    bool portability_enabled = false;
    bool draw_window = false;

    std::string application_name = "PP Application";
    unsigned int application_version = pp_make_version(1, 0, 0);

    std::vector<const char *> vk_layers;
    std::vector<const char *> vk_instance_extensions;
    std::vector<const char *> vk_device_extensions;

    vk::PhysicalDeviceFeatures vk_physical_device_features;
};

class WindowSettings : public Settings
{
  public:
    WindowSettings() = default;
    WindowSettings(const WindowSettings&) = default;
    WindowSettings(WindowSettings&&) = default;

    ~WindowSettings() = default;

    WindowSettings& operator = (const WindowSettings&) = default;
    WindowSettings& operator = (WindowSettings&&) = default;

    static WindowSettings default_values();

    float aspect_ratio() const;
    vk::Extent2D extent() const;

  public:
    bool initialized = false;

    std::string title = "PP Application";
    std::array<unsigned int, 2> size = { 1280, 720 };
    std::array<float, 2> scale = { 1.0f, 1.0f };

    vk::Format format = vk::Format::eB8G8R8A8Srgb;
    vk::ColorSpaceKHR color_space = vk::ColorSpaceKHR::eSrgbNonlinear;
    vk::PresentModeKHR present_mode = vk::PresentModeKHR::eMailbox;
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

#endif // physp_core_settings_decl_hpp