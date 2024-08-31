#include "tests/test_classes.hpp"

#include <catch2/catch_test_macros.hpp>

#include <iostream>

TEST_CASE( "new_array", "[unit][componentmanager]" )
{
  pp::ComponentManager manager;

  bool success = true;
  try
  {
    manager.newArrays<Test1, Test2>();
    manager.signature<Test1>();
  }
  catch (std::exception& e)
  {
    std::cout << "new_array: " << e.what() << '\n';
    success = false;
  }
  catch( ... )
  {
    std::cout << "new_array: unknown\n";
    success = false;
  }

  CHECK( success );
}

TEST_CASE( "remove_array", "[unit][componentmanager]" )
{
  pp::ComponentManager manager;
  manager.newArrays<Test1, Test2>();

  bool success = true;
  try
  {
    manager.removeArrays<Test1, Test2>();
  }
  catch (std::exception& e)
  {
    std::cout << "new_array: " << e.what() << '\n';
    success = false;
  }
  catch( ... )
  {
    std::cout << "new_array: unknown\n";
    success = false;
  }

  CHECK( success );
}

TEST_CASE( "add_datas", "[unit][componentmanager]" )
{
  pp::ComponentManager manager;
  manager.newArrays<Test1, Test2>();

  bool success = true;
  try
  {
    manager.addDatas(0, Test1(), Test2());
  }
  catch (std::exception& e)
  {
    std::cout << "new_array: " << e.what() << '\n';
    success = false;
  }
  catch( ... )
  {
    std::cout << "new_array: unknown\n";
    success = false;
  }

  CHECK( success );
}

TEST_CASE( "remove_datas", "[unit][componentmanager]" )
{
  pp::ComponentManager manager;
  manager.newArrays<Test1, Test2>();
  manager.addDatas(0, Test1(), Test2());

  bool success = true;
  try
  {
    manager.removeDatas<Test1, Test2>(0);
  }
  catch (std::exception& e)
  {
    std::cout << "new_array: " << e.what() << '\n';
    success = false;
  }
  catch( ... )
  {
    std::cout << "new_array: unknown\n";
    success = false;
  }

  CHECK( success );
}

TEST_CASE( "component", "[unit][componentmanager]" )
{
  pp::ComponentManager manager;
  manager.newArrays<Test1>();
  manager.addDatas(0, Test1());

  Test1 data;

  bool success = true;
  try
  {
    data = manager.component<Test1>(0);
  }
  catch (std::exception& e)
  {
    std::cout << "new_array: " << e.what() << '\n';
    success = false;
  }
  catch( ... )
  {
    std::cout << "new_array: unknown\n";
    success = false;
  }

  REQUIRE( success );
  CHECK( data.a == 0 );
}

TEST_CASE( "component_signature", "[unit][componentmanager]" )
{
  pp::ComponentManager manager;
  manager.newArrays<Test1>();

  pp::Signature signature;

  bool success = true;
  try
  {
    signature = manager.signature<Test1>();
  }
  catch (std::exception& e)
  {
    std::cout << "component_signature: " << e.what() << '\n';
    success = false;
  }
  catch( ... )
  {
    std::cout << "component_signature: unknown\n";
    success = false;
  }

  REQUIRE( success );
  CHECK( signature == pp::Signature(0x1ul) );
}