#ifndef physp_core_interface_decl_hpp
#define physp_core_interface_decl_hpp

#include "src/core/include/iwindow.hpp"
#include <memory>

namespace pp
{

class IInterface
{
  public:
    IInterface() = default;
    IInterface(IInterface&) = delete;
    IInterface(IInterface&&) = delete;

    virtual ~IInterface() = default;

    IInterface& operator = (IInterface&) = delete;
    IInterface& operator = (IInterface&&) = delete;

    virtual bool should_close() const = 0;
    virtual void poll_events() const = 0;
    virtual const vk::raii::SurfaceKHR& surface() const = 0;
    virtual const vk::Image& image(unsigned long) const = 0;
    virtual const vk::raii::ImageView& image_view(unsigned long) const = 0;

    virtual unsigned long next_image_index(const vk::raii::Semaphore&) = 0;
    virtual void add_keybind(Key, std::function<void()>) = 0;
    virtual void add_keybind(std::vector<Key>&&, std::function<void()>) = 0;
    virtual void create_surface(const vk::raii::Instance&) = 0;
    virtual void load(std::shared_ptr<Device>) = 0;
};

template <typename T>
class Interface : public IInterface
{
  static_assert(std::is_base_of<IWindow, T>::value,
    "pp::Interface: attempted to create an interface with non-window class"
  );

  public:
    Interface() = default;
    ~Interface() = default;

    bool should_close() const override;
    void poll_events() const override;
    const vk::raii::SurfaceKHR& surface() const override;
    const vk::Image& image(unsigned long) const override;
    const vk::raii::ImageView& image_view(unsigned long) const override;

    unsigned long next_image_index(const vk::raii::Semaphore&) override;
    void add_keybind(Key, std::function<void()>) override;
    void add_keybind(std::vector<Key>&&, std::function<void()>) override;
    void create_surface(const vk::raii::Instance&) override;
    void load(std::shared_ptr<Device>) override;

  private:
    T window;
};

} // namespace pp

#endif // physp_core_interface_decl_hpp