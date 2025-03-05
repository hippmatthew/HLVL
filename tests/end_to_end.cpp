#define hlvl_tests
#include "src/core/include/context.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE( "end_to_end", "[endtoend]" ) {
  hlvl::Context context;

  CHECK( context.window() != nullptr );
  CHECK( context.instance() != nullptr );
  CHECK( context.surface() != nullptr );
  CHECK( context.physicalDevice() != nullptr );
  CHECK( context.device() != nullptr );
  CHECK( !context.queueFamilies().empty() );

  hlvl_loop_start {

  } hlvl_loop_end(true);
}