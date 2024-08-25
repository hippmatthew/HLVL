#include "physp/physp.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE( "new_entity", "[unit][controller]" )
{
  pp::ECSController controller;

  pp::Entity entity;
  for (unsigned long i = 0; i < 5; ++i)
    entity = controller.new_entity();

  CHECK( entity == 4 );
}