#include "src/hlvl/include/context.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE( "end_to_end", "[endtoend]" ) {
  hlvl::Context context;

  unsigned int counter = 0;

  hlvl_loop_start {

    ++counter;

  } hlvl_loop_end(!context || counter == 10);

  CHECK( counter == 10 );
}