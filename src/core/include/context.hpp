#ifndef physp_core_context_hpp
#define physp_core_context_hpp

#include "src/core/include/context_decl.hpp"

namespace pp
{

template <typename T>
void Context::set_interface()
{
  static_assert(std::is_base_of<IWindow, T>::value,
    "pp::Context: attempted to set interface to non-window type"
  );

  p_interface = std::make_shared<Interface<T>>();
}

} // namespace pp

#endif // physp_core_context_hpp