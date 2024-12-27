#include "hlvl/hlvl.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE( "new_entity", "[unit][entitymanager]" )
{
  hlvl::EntityManager entityManager;

  std::vector<hlvl::Entity> entities;
  for (unsigned long i = 0; i < 5; ++i)
  {
    entities.emplace_back(entityManager.newEntity());
    CHECK( entities[i] == i );
  }
}

TEST_CASE( "signature", "[unit][entitymanager]" )
{
  hlvl::EntityManager entityManager;

  hlvl::Entity entity = entityManager.newEntity();
  hlvl::Signature& signature = entityManager.signature(entity);
  signature = 1;

  CHECK( entityManager.signature(entity) == 1 );
}