#pragma once

#include "../build/include/hlvl/hlvl.hpp"

class Test1 : public hlvl::Settings
{
  public:
    Test1() = default;
    Test1(const Test1&) = default;
    Test1(Test1&&) = default;

    void reset_to_default() { a = 0; }

    Test1& operator = (const Test1&) = default;
    Test1& operator = (Test1&&) = default;

  public:
    int a = 0;
};

class Test2 : public hlvl::Settings
{
  public:
    void reset_to_default() { b = 0; };

  public:
    int b = 0;
};

class KeyTest
{
  public:
    static KeyTest * instance;

  public:
    KeyTest()
    {
      KeyTest::instance = this;
      a = both = false;
    }

    static void pressA() { KeyTest::instance->a = true; }
    static void pressAB() { KeyTest::instance->both = true; }

  public:
    bool a, both;

};

struct Vertex
{
  unsigned int x, y, z;
};

class System1 : public hlvl::ISystem
{
  public:
    void run()
    {
      for (const auto& entity : entities)
      {
        auto& resource = component<hlvl::Resource<Test1>>(entity);

        Test1 data = *resource;
        data.a += 1;
        resource = data;
      }
    }
};

class System2 : public hlvl::ISystem
{
  public:
    void run()
    {
      for (const auto& entity : entities)
      {
        auto& resource = component<hlvl::Resource<Test1>>(entity);
        auto& multiplier = component<int>(entity);

        Test1 data = *resource;
        data.a = (data.a * multiplier) % 1024;
        resource = data;
      }
    }
};

inline void reset_settings()
{
  hlvl_general_settings = hlvl::GeneralSettings::default_values();
  hlvl_window_settings = hlvl::WindowSettings::default_values();
}