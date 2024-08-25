#ifndef physp_tests_test_classes_hpp
#define physp_tests_test_classes_hpp

#include "physp/physp.hpp"

class Test1 : public pp::Settings
{
  public:
    void reset_to_default() { a = 0; }

  public:
    int a = 0;
};

class Test2 : public pp::Settings
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

inline void reset_settings()
{
  pp_general_settings = pp::GeneralSettings::default_values();
  pp_window_settings = pp::WindowSettings::default_values();
}

#endif // physp_tests_test_classes_hpp