#ifndef physp_core_context_decl_hpp
#define physp_core_context_decl_hpp

#include "src/core/include/interface.hpp"
#include "src/core/include/device.hpp"

#ifndef physp_vulkan_include
#define physp_vulkan_include

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan_raii.hpp>

#endif // physp_vulkan_include

namespace pp
{

class Context
{
  public:
    Context() = default;
    Context(Context&) = delete;
    Context(Context&&) = delete;

    ~Context() = default;

    Context& operator = (Context&) = delete;
    Context& operator = (Context&&) = delete;

    void initialize(void * p_next = nullptr);

    template <typename T>
    void set_interface();

  private:
    void createInstance();

  private:
    std::shared_ptr<IInterface> p_interface = nullptr;
    std::shared_ptr<Device> p_device = nullptr;

    vk::raii::Instance vk_instance = nullptr;
};

} // namespace pp

#endif // physp_core_context_decl_hpp
