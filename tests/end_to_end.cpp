#include <catch2/catch_session.hpp>
#include <physp/physp.hpp>

int main()
{
  int tests = Catch::Session().run();

  pp::Context context;
  context.initalize();

  return tests;
}