#include "test_classes.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE( "retrieve", "[unit][settings]" )
{
  SECTION( "valid" )
  {
    auto&& s = hlvl_general_settings;

    CHECK( s.application_name == "HLVL Application" );
    CHECK( s.application_version == hlvl_make_version(1, 0, 0) );
  }

  SECTION( "invalid" )
  {
    bool expected_failure = false;
    bool unexpected_failure = false;

    try
    {
      Test1 s = hlvl_settings_manager.settings<Test1>();
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
    hlvl_settings_manager.add_settings<Test1, Test2>();

    Test1 s1 = hlvl_settings_manager.settings<Test1>();
    Test2 s2 = hlvl_settings_manager.settings<Test2>();

    CHECK( s1.a == 0 );
    CHECK( s2.b == 0 );
  }

  SECTION( "reinsertion" )
  {
    bool expected_failure = false;
    bool unexpected_failure = false;

    try
    {
      hlvl_settings_manager.add_settings<Test1, Test2>();
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
    hlvl_settings_manager.remove_settings<Test1, Test2>();

    bool expected_failure = false;
    bool unexpected_failure = false;

    try
    {
      auto s = hlvl_settings_manager.settings<Test1>();
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
      auto s = hlvl_settings_manager.settings<Test2>();
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
  auto& s = hlvl_general_settings;
  s.application_name = "Test";
  s.application_version = 1u;

  auto& u = hlvl_general_settings;

  CHECK( u.application_name == s.application_name );
  CHECK( u.application_version == s.application_version );
}

TEST_CASE( "persistence", "[unit][settings]" )
{
  auto& s = hlvl_general_settings;

  CHECK( s.application_name == "Test" );
  CHECK( s.application_version == 1u );
}

TEST_CASE( "general", "[unit][settings]" )
{
  auto& s_general = hlvl_general_settings;

  SECTION( "add_layers" )
  {
    s_general.vk_layers.clear();

    std::vector<const char *> testNames = { "name1", "name2", "name3" };
    s_general.add_layers(std::move(testNames));

    bool success = true;

    for (unsigned long i = 0; i < 3; ++i)
    {
      if (std::string(s_general.vk_layers[i]) != testNames[i])
      {
        success = false;
        break;
      }
    }

    CHECK( success );
  }

  SECTION( "add_instance_extensions" )
  {
    s_general.vk_instance_extensions.clear();

    std::vector<const char *> testNames = { "name1", "name2", "name3" };
    s_general.add_instance_extensions(std::move(testNames));

    bool success = true;

    for (unsigned long i = 0; i < 3; ++i)
    {
      if (std::string(s_general.vk_instance_extensions[i]) != testNames[i])
      {
        success = false;
        break;
      }
    }

    CHECK( success );
  }

  SECTION( "add_device_extensions" )
  {
    s_general.vk_device_extensions.clear();

    std::vector<const char *> testNames = { "name1", "name2", "name3" };
    s_general.add_device_extensions(std::move(testNames));

    bool success = true;

    for (unsigned long i = 0; i < 3; ++i)
    {
      if (std::string(s_general.vk_device_extensions[i]) != testNames[i])
      {
        success = false;
        break;
      }
    }

    CHECK( success );
  }

  SECTION( "defaults" )
  {
    s_general = hlvl::GeneralSettings::default_values();

    CHECK( s_general.application_name == "HLVL Application" );
    CHECK( s_general.application_version == hlvl_make_version(1, 0, 0) );
    CHECK( s_general.vk_layers.empty() );
    CHECK( s_general.vk_instance_extensions.empty() );
    CHECK( s_general.vk_device_extensions.empty() );
    CHECK( s_general.vk_physical_device_features == vk::PhysicalDeviceFeatures{} );
    CHECK( s_general.portability_enabled == false );
  }
}

TEST_CASE( "window", "[unit][settings]" )
{
  hlvl::WindowSettings& s_window = hlvl_window_settings;

  SECTION( "aspect_ratio" )
  {
    s_window.size = { 2, 2 };
    s_window.scale = { 2.0f, 2.0f };

    CHECK( s_window.aspect_ratio() == 1.0f );
  }

  SECTION( "extent" )
  {
    CHECK( s_window.extent().width == 4 );
    CHECK( s_window.extent().height == 4 );
  }

  SECTION( "defaults" )
  {
    s_window = hlvl::WindowSettings::default_values();

    CHECK( s_window.title == "HLVL Application" );
    CHECK( s_window.size == std::array<unsigned int, 2>{ 1280, 720 } );
    CHECK( s_window.scale == std::array<float, 2>{ 1.0f, 1.0f } );
    CHECK( s_window.initialized == false );
  }
}