#include "physp/physp.hpp"
#include "physp/physp_decl.hpp"
#include "tests/test_classes.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_session.hpp>

#include <exception>
#include <iostream>

TEST_CASE( "run_test", "[endtoend]" )
{
  reset_settings();

  pp_general_settings.vk_layers.emplace_back("VK_LAYER_KHRONOS_validation");
  pp::Resource<unsigned int> counter(0, pp::ResourceType::uniform);
  unsigned long iterations = 1000;

  bool success = true;
  try
  {
    pp::Context context;
    context.initialize();

    auto& allocator = context.allocator();

    allocator.new_allocation({ pp::Locality::device, { &counter }});

    unsigned int iteration = 0;

    pp_loop_start

    counter = *counter + 1;
    ++iteration;

    pp_loop_end( iteration == iterations );

    allocator.wait();
  }
  catch( std::exception& e)
  {
    std::cout << "run_test: " << e.what() << '\n';
    success = false;
  }
  catch( ... )
  {
    std::cout << "run_test: unknown\n";
    success = false;
  }

  CHECK( success );
  CHECK( *counter == iterations );
}

int main()
{
  return Catch::Session().run();
}