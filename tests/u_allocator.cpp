#include "tests/test_classes.hpp"

#include <catch2/catch_test_macros.hpp>

#include <iostream>
#include <vector>

TEST_CASE( "buffer_allocation", "[unit][allocator]" )
{
  reset_settings();

  pp_general_settings.vk_layers.emplace_back("VK_LAYER_KHRONOS_validation");

  pp::Context context;
  context.initialize();

  auto& allocator = context.allocator();

  SECTION( "single_resource_allocation" )
  {
    pp::Resource<std::vector<Vertex>> resource({{ 1, 2, 3 }}, pp::ResourceType::vertex);

    bool success = true;
    try
    {
      allocator.new_allocation({ pp::Locality::host, { &resource } });
    }
    catch (std::exception& e)
    {
      std::cout << "buffer_allocation::single_resource_allocation: " << e.what() << '\n';
      success = false;
    }
    catch ( ... )
    {
      std::cout << "buffer_allocation::single_resource_allocation: unknown\n";
      success = false;
    }

    CHECK( success );
  }

  SECTION( "multiple_resource_allocation" )
  {
    pp::Resource<std::vector<Vertex>> resource1({{ 1, 2, 3 }}, pp::ResourceType::vertex);
    pp::Resource<std::vector<unsigned int>> resource2({ 1, 2, 3 }, pp::ResourceType::index);
    pp::Resource<Test1> resource3(Test1{}, pp::ResourceType::uniform);
    pp::Resource<Test2> resource4(Test2{}, pp::ResourceType::uniform);

    bool success = true;
    try
    {
      allocator.new_allocation({ pp::Locality::device, { &resource1, &resource2, &resource3, &resource4 } });
    }
    catch (std::exception& e)
    {
      std::cout << "buffer_allocation::multiple_resource_allocation: " << e.what() << '\n';
      success = false;
    }
    catch ( ... )
    {
      std::cout << "buffer_allocation::multiple_resource_allocation: unknown\n";
      success = false;
    }

    CHECK( success );
  }
}

TEST_CASE( "buffer_access", "[unit][allocator]" )
{
  reset_settings();

  pp_general_settings.vk_layers.emplace_back("VK_LAYER_KHRONOS_validation");

  pp::Context context;
  context.initialize();

  auto& allocator = context.allocator();

  pp::Resource<Test1> resource(Test1{}, pp::ResourceType::uniform);
  allocator.new_allocation({ pp::Locality::host, { &resource } });

  CHECK( *resource.buffer() != nullptr );
}

TEST_CASE( "buffer_update", "[unit][allocator]" )
{
  reset_settings();

  pp_general_settings.vk_layers.emplace_back("VK_LAYER_KHRONOS_validation");

  pp::Context context;
  context.initialize();

  auto& allocator = context.allocator();

  SECTION( "host_locality" )
  {
    pp::Resource<Test1> resource(Test1{}, pp::ResourceType::uniform);
    allocator.new_allocation({ pp::Locality::host, { &resource } });

    Test1 test;
    test.a = 1;

    bool success = true;
    try
    {
      resource = test;
    }
    catch (std::exception& e)
    {
      std::cout << "buffer_update::host_locality: " << e.what() << '\n';
      success = false;
    }
    catch ( ... )
    {
      std::cout << "buffer_update::host_locality: unknown\n";
      success = false;
    }

    CHECK( success );
  }

  SECTION( "device_locality" )
  {
    pp::Resource<Test1> resource(Test1{}, pp::ResourceType::uniform);
    allocator.new_allocation({ pp::Locality::device, { &resource } });

    Test1 test;
    test.a = 1;

    bool success = true;
    try
    {
      resource = test;
      allocator.wait();
    }
    catch (std::exception& e)
    {
      std::cout << "buffer_update::device_locality: " << e.what() << '\n';
      success = false;
    }
    catch ( ... )
    {
      std::cout << "buffer_update::device_locality: unknown\n";
      success = false;
    }

    CHECK( success );
  }
}