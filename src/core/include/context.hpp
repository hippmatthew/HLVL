#pragma once

#include "context_decl.hpp"

namespace hlvl
{

template <typename T>
Context& Context::set_interface()
{
  static_assert(std::is_base_of<IWindow, T>::value,
    "hlvl::Context: attempted to set interface to non-window type"
  );

  p_interface = std::make_shared<Interface<T>>();
  return *this;
}

} // namespace hlvl