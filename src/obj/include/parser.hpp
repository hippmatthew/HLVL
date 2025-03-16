#pragma once

#include "src/core/include/vertex.hpp"

#include <string>
#include <vector>

namespace obj {

class ObjParser {
  using Result = std::pair<std::vector<hlvl::Vertex>, std::vector<unsigned int>>;

  public:
    ObjParser() = delete;
    ObjParser(const ObjParser&) = default;
    ObjParser(ObjParser&&) = default;

    static Result parse(std::string path);
};

} // namespace obj