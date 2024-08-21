#include "physp/physp.hpp"
#include "physp/physp_decl.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_session.hpp>

#include <exception>
#include <iostream>

TEST_CASE( "application test", "[endtoend]" )
{
  pp_settings_manager.settings<pp::General>() = pp::General::default_values();
  pp_settings_manager.settings<pp::Window>() = pp::Window::default_values();

  pp_settings_manager.settings<pp::General>().vk_layers.emplace_back("VK_LAYER_KHRONOS_validation");

  bool success = true;
  try
  {
    pp::Context context;
    context.initialize();
  }
  catch( std::exception& e)
  {
    std::cout << "exception caught: " << e.what() << '\n';
    success = false;
  }
  catch( ... )
  {
    success = false;
  }

  CHECK( success );
}

int main()
{
  return Catch::Session().run();
}