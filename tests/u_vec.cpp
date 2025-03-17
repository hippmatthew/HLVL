#include "src/linalg/include/vec.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE( "zero_vec", "[unit][vec]" ) {
  CHECK( la::vec<3>::zero() == la::vec<3>{ 0, 0, 0 } );
}

TEST_CASE( "scalar_mult", "[unit][vec]" ) {
  CHECK( 3 * la::vec<2>{ 1, 2 } == la::vec<2>{ 3, 6 });
}

TEST_CASE( "scalar_div", "[unit][vec]" ) {
  CHECK( la::vec<3, double>{ 3, 6, 9 } / 3 == la::vec<3, double>{ 1, 2, 3 } );
}

TEST_CASE( "dot", "[unit][vec]" ) {
  la::vec<4, int> u = { 1, 1, 0, 1 };
  la::vec<4, int> v = { 1, 0, 1, 1 };

  CHECK( u * v == v * u );
  CHECK( u * v == 2 );
}

TEST_CASE( "cross", "[unit][vec]" ) {
  CHECK( la::vec<3>{ 1, 0, 0 }.cross(la::vec<3>{ 0, 1, 0 }) == la::vec<3>{ 0, 0, 1 } );
  CHECK( la::vec<3>{ 0, 1, 0 }.cross(la::vec<3>{ 0, 0, 1 }) == la::vec<3>{ 1, 0, 0 });
  CHECK( la::vec<3>{ 0, 0, 1 }.cross(la::vec<3>{ 1, 0, 0 }) == la::vec<3>{ 0, 1, 0 });
}

TEST_CASE( "magnitude", "[unit][vec]" ) {
  CHECK( la::vec<3, unsigned int>{ 4, 0, 0 }.magnitude() == 4.0 );
}

TEST_CASE( "normalized", "[unit][vec]" ) {
  CHECK( la::vec<3>{ 4, 0, 0 }.normalized() == la::vec<3>{ 1, 0, 0 } );
}