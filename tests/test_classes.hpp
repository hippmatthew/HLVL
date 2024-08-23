#ifndef physp_tests_test_classes_hpp
#define physp_tests_test_classes_hpp

#include "physp/physp.hpp"
#include "physp/physp_decl.hpp"

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