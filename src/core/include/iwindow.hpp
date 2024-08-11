#ifndef physp_core_iwindow_hpp
#define physp_core_iwindow_hpp

#include "src/core/include/iwindow_decl.hpp"
#include <stdexcept>

namespace pp
{

// template <typename T>
// void IWindow::add_binding(Key k)
// {
//   static_assert(std::is_base_of<KeyCallback, T>::value,
//     "pp::IWindow: attempted to add non-keycallback type to the key map"
//   );

//   if (keyMap.find(binding) != keyMap.end())
//   {
//     throw std::runtime_error("pp::IWindow: attempted to add keycallback that already exists");
//   }
// }

} // namespace pp

#endif // physp_core_iwindow_hpp