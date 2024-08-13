#include "physp/physp_decl.hpp"
#include <catch2/catch_session.hpp>
#include <physp/physp.hpp>

int main()
{
  int tests = Catch::Session().run();

  pp_settings_manager.settings<pp::General>().add_layers({ "VK_LAYER_KHRONOS_validation" });

  auto v = pp::windows::GLFW::instance_extensions();

  // pp::Context context;
  // context.initialize();

  return tests;
}