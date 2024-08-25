#include "physp/physp.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE( "new_entity", "[unit][entitymanager]" )
{
  pp::EntityManager entityManager;

  std::vector<pp::Entity> entities;
  for (unsigned long i = 0; i < 5; ++i)
  {
    entities.emplace_back(entityManager.newEntity());
    CHECK( entities[i] == i );
  }
}

TEST_CASE( "signature", "[unit][entitymanager]" )
{
  pp::EntityManager entityManager;

  pp::Entity entity = entityManager.newEntity();
  pp::Signature& signature = entityManager.signature(entity);
  signature = 1;

  CHECK( entityManager.signature(entity) == 1 );
}