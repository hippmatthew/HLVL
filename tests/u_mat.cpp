#include "src/linalg/include/mat.hpp"

#include <catch2/catch_test_macros.hpp>

#include <numbers>

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

TEST_CASE( "rotate", "[unit][mat]" ) {
  la::mat<4> x = la::mat<4>::rotation({ std::numbers::pi / 2, 0, 0 });
  la::mat<4> y = la::mat<4>::rotation({ 0, std::numbers::pi / 2, 0 });
  la::mat<4> z = la::mat<4>::rotation({ 0, 0, std::numbers::pi / 2 });

  CHECK( x * x.transpose() == la::mat<4>::identity() );
  CHECK( y * y.transpose() == la::mat<4>::identity() );
  CHECK( z * z.transpose() == la::mat<4>::identity() );
}

TEST_CASE( "translate", "[unit][mat]" ) {
  CHECK( la::mat<4>::translation({ 1, 2, 3 }) == la::mat<4>{
    { 1, 0, 0, 1 },
    { 0, 1, 0, 2 },
    { 0, 0, 1, 3 },
    { 0, 0, 0, 1 }
  });
}

TEST_CASE( "scale", "[unit][mat]" ) {
  CHECK( la::mat<4>::scale({ 2, 3, 4 }) == la::mat<4>{
    { 2, 0, 0, 0 },
    { 0, 3, 0, 0 },
    { 0, 0, 4, 0 },
    { 0, 0, 0, 1 }
  });
}