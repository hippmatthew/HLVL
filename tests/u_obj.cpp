#include "src/obj/include/parser.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE( "parse_obj", "[unit][obj]" ) {
  auto [vertices, indices] = obj::ObjParser::parse("../tests/dat/cube.obj");

  std::vector<unsigned int> expectedIndices = {
    0, 1, 2, 2, 3, 0,
    4, 5, 6, 6, 7, 4,
    8, 9, 10, 10, 11, 8,
    12, 13, 14, 14, 15, 12,
    4, 13, 16, 16, 17, 4,
    18, 19, 6, 6, 15, 18
  };

  std::vector<hlvl::Vertex> expectedVertices = {
    {{ 0.5, 0.5, 0.5 }, { 1.0, 0.0 }},
    {{ 0.5, -0.5, 0.5 }, { 0.0, 0.0 }},
    {{ -0.5, -0.5, 0.5 }, { 0.0, 1.0 }},
    {{ -0.5, 0.5, 0.5 }, { 1.0, 1.0 }},
    {{ 0.5, 0.5, -0.5 }, { 1.0, 0.0 }},
    {{ 0.5, 0.5, 0.5 }, { 0.0, 0.0 }},
    {{ -0.5, 0.5, 0.5 }, { 0.0, 1.0 }},
    {{ -0.5, 0.5, -0.5 }, { 1.0, 1.0 }},
    {{ 0.5, -0.5, -0.5 }, { 1.0, 0.0 }},
    {{ 0.5, 0.5, -0.5 }, { 0.0, 0.0 }},
    {{ -0.5, 0.5, -0.5 }, { 0.0, 1.0 }},
    {{ -0.5, -0.5, -0.5 }, { 1.0, 1.0 }},
    {{ 0.5, -0.5, 0.5 }, { 1.0, 0.0 }},
    {{ 0.5, -0.5, -0.5 }, { 0.0, 0.0 }},
    {{ -0.5, -0.5, -0.5 }, { 0.0, 1.0 }},
    {{ -0.5, -0.5, 0.5 }, { 1.0, 1.0 }},
    {{ 0.5, -0.5, 0.5 }, { 0.0, 1.0 }},
    {{ 0.5, 0.5, 0.5 }, { 1.0, 1.0 }},
    {{ -0.5, -0.5, -0.5 }, { 1.0, 0.0 }},
    {{ -0.5, 0.5, -0.5 }, { 0.0, 0.0 }}
  };

  REQUIRE( vertices.size() == expectedVertices.size() );
  REQUIRE( indices.size() ==  expectedIndices.size() );

  CHECK( vertices == expectedVertices );
  CHECK( indices == expectedIndices );
}