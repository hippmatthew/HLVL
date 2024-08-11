#ifndef physp_core_context_decl_hpp
#define physp_core_context_decl_hpp

#include "src/core/include/settings.hpp"
#include "src/core/include/interface_decl.hpp"

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

    template <typename T>
    void set_interface();

  private:
    void createInstance();

  private:
    std::shared_ptr<IInterface> p_interface = nullptr;

    vk::raii::Instance vk_instance = nullptr;
};

} // namespace pp

#endif // physp_core_context_decl_hpp