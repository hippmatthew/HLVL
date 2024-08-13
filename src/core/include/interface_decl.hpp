#ifndef physp_core_interface_decl_hpp
#define physp_core_interface_decl_hpp

#include "src/core/include/iwindow.hpp"

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
    virtual const vk::raii::SurfaceKHR& surface() = 0;
    virtual void create_surface(const vk::raii::Instance&) = 0;
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

    bool should_close() const;
    void poll_events() const;
    const vk::raii::SurfaceKHR& surface() const;

    void create_surface(const vk::raii::Instance&);

    // template <typename K>
    // void add_binding(Key);

  private:
    T window;
};

} // namespace pp

#endif // physp_core_interface_decl_hpp