#include "test_classes.hpp"

#include <catch2/catch_test_macros.hpp>

#include <iostream>

TEST_CASE( "add_systems", "[unit][systemmanager]" )
{
  hlvl::ComponentManager componentManager;
  hlvl::SystemManager systemManager;

  bool success = true;
  try
  {
    systemManager.addSystems<System1, System2>(&componentManager);
  }
  catch (std::exception& e)
  {
    std::cout << "add_systems: " << e.what() << '\n';
    success = false;
  }
  catch ( ... )
  {
    std::cout << "add_systems: unknown\n";
    success = false;
  }

  CHECK( success );
}

TEST_CASE( "remove_systems", "[unit][systemmanager]" )
{
  hlvl::ComponentManager componentManager;
  hlvl::SystemManager systemManager;

  systemManager.addSystems<System1, System2>(&componentManager);

  bool success = true;
  try
  {
    systemManager.removeSystems<System1, System2>();
  }
  catch (std::exception& e)
  {
    std::cout << "remove_systems: " << e.what() << '\n';
    success = false;
  }
  catch ( ... )
  {
    std::cout << "remove_systems: unknown\n";
    success = false;
  }

  CHECK( success );
}

TEST_CASE( "add_entities", "[unit][systemmanager]" )
{
  hlvl::ComponentManager componentManager;
  hlvl::SystemManager systemManager;

  systemManager.addSystems<System1>(&componentManager);

  bool success = true;
  try
  {
    systemManager.addEntities<System1>({ 0, 1, 2, 3 });
  }
  catch (std::exception& e)
  {
    std::cout << "add_entities: " << e.what() << '\n';
    success = false;
  }
  catch ( ... )
  {
    std::cout << "add_entities: unknown\n";
    success = false;
  }

  CHECK( success );
}

TEST_CASE( "remove_entities", "[unit][systemmanager]" )
{
  hlvl::ComponentManager componentManager;
  hlvl::SystemManager systemManager;

  systemManager.addSystems<System1>(&componentManager);
  systemManager.addEntities<System1>({ 0, 1, 2, 3 });

  bool success = true;
  try
  {
    systemManager.removeEntities<System1>({ 0, 1, 2, 3 });
  }
  catch (std::exception& e)
  {
    std::cout << "add_entities: " << e.what() << '\n';
    success = false;
  }
  catch ( ... )
  {
    std::cout << "add_entities: unknown\n";
    success = false;
  }

  CHECK( success );
}