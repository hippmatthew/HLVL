#pragma once

#include "allocator.hpp"
#include "interface.hpp"
#include "ecscontroller.hpp"

namespace hlvl
{

class Context
{
  public:
    Context();
    Context(Context&) = delete;
    Context(Context&&) = delete;

    ~Context();

    Context& operator = (Context&) = delete;
    Context& operator = (Context&&) = delete;

    Allocator& allocator();
    ECSController& ecs_controller();

    bool should_close() const;
    void poll_events() const;

    Context& initialize(void * p_next = nullptr);
    Context& add_keybind(Key, std::function<void()>);
    Context& add_keybind(std::vector<Key>&&, std::function<void()>);

    template <typename T>
    Context& set_interface();

  private:
    void createInstance();

  protected:
    ECSController ecsController;

    std::shared_ptr<IInterface> p_interface = nullptr;
    std::shared_ptr<Device> p_device = nullptr;
    std::shared_ptr<Allocator> p_allocator = nullptr;

    vk::raii::Instance vk_instance = nullptr;
};

} // namespace hlvl
