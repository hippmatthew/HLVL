#include "tests/test_classes.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE( "retrieve", "[unit][settings]" )
{
  SECTION( "valid" )
  {
    pp::General& s = pp_settings_manager.settings<pp::General>();

    CHECK( s.application_name == "PP Application" );
    CHECK( s.application_version == pp_make_version(1, 0, 0) );
  }

  SECTION( "invalid" )
  {
    bool expected_failure = false;
    bool unexpected_failure = false;

    try
    {
      Test1 s = pp_settings_manager.settings<Test1>();
    }
    catch (const std::runtime_error& err)
    {
      expected_failure = true;
    }
    catch ( ... )
    {
      unexpected_failure = true;
    }

    CHECK( expected_failure );
    CHECK( !unexpected_failure );
  }
}

TEST_CASE( "add", "[unit][settings]" )
{
  SECTION( "valid" )
  {
    pp_settings_manager.add_settings<Test1, Test2>();

    Test1 s1 = pp_settings_manager.settings<Test1>();
    Test2 s2 = pp_settings_manager.settings<Test2>();

    CHECK( s1.a == 0 );
    CHECK( s2.b == 0 );
  }

  SECTION( "reinsertion" )
  {
    bool expected_failure = false;
    bool unexpected_failure = false;

    try
    {
      pp_settings_manager.add_settings<Test1, Test2>();
    }
    catch (const std::runtime_error& err)
    {
      expected_failure = true;
    }
    catch ( ... )
    {
      unexpected_failure = false;
    }

    CHECK( expected_failure );
    CHECK( !unexpected_failure );
  }
}

TEST_CASE( "remove", "[unit][settings]" )
{
  SECTION( "valid" )
  {
    pp_settings_manager.remove_settings<Test1, Test2>();

    bool expected_failure = false;
    bool unexpected_failure = false;

    try
    {
      auto s = pp_settings_manager.settings<Test1>();
    }
    catch (const std::runtime_error& err)
    {
      expected_failure = true;
    }
    catch ( ... )
    {
      unexpected_failure = true;
    }

    CHECK( expected_failure );
    CHECK( !unexpected_failure );

    expected_failure = unexpected_failure = false;

    try
    {
      auto s = pp_settings_manager.settings<Test2>();
    }
    catch (const std::runtime_error& err)
    {
      expected_failure = true;
    }
    catch ( ... )
    {
      unexpected_failure = true;
    }

    CHECK( expected_failure );
    CHECK( !unexpected_failure );
  }
}

TEST_CASE( "mutation", "[unit][settings]" )
{
  pp::General& s = pp_settings_manager.settings<pp::General>();
  s.application_name = "Test";
  s.application_version = 1u;

  pp::General& u = pp_settings_manager.settings<pp::General>();

  CHECK( u.application_name == s.application_name );
  CHECK( u.application_version == s.application_version );
}

TEST_CASE( "persistence", "[unit][settings]" )
{
  pp::General& s = pp_settings_manager.settings<pp::General>();

  CHECK( s.application_name == "Test" );
  CHECK( s.application_version == 1u );
}

TEST_CASE( "default", "[unit][settings]" )
{
  pp::General& s = pp_settings_manager.settings<pp::General>();
  s = pp::General::default_values();

  CHECK( s.application_name == "PP Application" );
  CHECK( s.application_version == pp_make_version(1, 0, 0) );
}