#ifndef physp_core_context_hpp
#define physp_core_context_hpp

#include <vulkan/vulkan_raii.hpp>

namespace pp
{

class Context
{
  public:
    Context();
    Context(Context&) = delete;
    Context(Context&&) = delete;

    ~Context() = default;

    Context& operator = (Context&) = delete;
    Context& operator = (Context&&) = delete;

  private:
    void createInstance();

  private:
    vk::raii::Instance vk_instance = nullptr;
};

} // namespace pp

#endif // physp_core_context_hpp