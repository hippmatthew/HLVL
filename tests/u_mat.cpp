#include "src/linalg/include/mat.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE( "identity_mat", "[unit][mat]" ) {
  la::mat<3> expected = {
    { 1, 0, 0 },
    { 0, 1, 0 },
    { 0, 0, 1 }
  };

  CHECK( la::mat<3>::identity() == expected );
}

TEST_CASE( "scalar_mult", "[unit][mat]" ) {
  la::mat<3> expected = {
    { 3, 0, 0 },
    { 0, 3, 0 },
    { 0, 0, 3 }
  };
  CHECK( 3 * la::mat<3>::identity() == expected );
}

TEST_CASE( "scalar_div", "[unit][mat]" ) {
  la::mat<3> m = {
    { 3, 0, 0 },
    { 0, 3, 0 },
    { 0, 0, 3 }
  };
  CHECK( m / 3 == la::mat<3>::identity() );
}

TEST_CASE( "vec_mult", "[unit][mat]" ) {
  la::mat<3> m = {
    { 3, 0, 0 },
    { 0, 5, 0 },
    { 0, 0, 1 }
  };

  CHECK( m * la::vec<3>{ 2, 3, 0 } == la::vec<3>{ 6, 15, 0 } );
}

TEST_CASE( "mat_mult", "[unit][mat]" ) {
  la::mat<2> sx = {
    { 0, 1 },
    { 1, 0 }
  };

  CHECK( sx * sx == la::mat<2>::identity() );
}

TEST_CASE( "transpose", "[unit][mat]" ) {
  la::mat<3> m {
    { 0, 1, 1 },
    { 0, 0, 1 },
    { 0, 0, 0 }
  };

  la::mat<3> expected {
    { 0, 0, 0 },
    { 1, 0, 0 },
    { 1, 1, 0 }
  };

  CHECK( m.transpose() == expected );
}