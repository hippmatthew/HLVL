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

#endif // physp_tests_test_classes_hpp