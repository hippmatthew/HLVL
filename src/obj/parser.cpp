#include "src/obj/include/parser.hpp"

#include <fstream>
#include <map>
#include <sstream>
#include <stdexcept>

namespace obj {

ObjParser::Result ObjParser::parse(std::string path) {
  std::vector<hlvl::Vertex> vertices;
  std::vector<unsigned int> indices;

  std::vector<la::vec<3>> positions;
  std::vector<la::vec<2>> uvs;

  std::map<std::pair<unsigned int, unsigned int>, unsigned int> indexMap;

  std::ifstream file(path);
  if (!file) throw std::runtime_error("hlvl: filed to open obj file");

  std::string line;
  while (std::getline(file, line)) {
    std::istringstream ss(line);

    std::string key;
    ss >> key;

    if (key == "v") {
      la::vec<3> position;
      ss >> position[0] >> position[1] >> position[2];
      positions.emplace_back(std::move(position));
    }
    else if (key == "vt") {
      la::vec<2> uv;
      ss >> uv[0] >> uv[1];
      uvs.emplace_back(std::move(uv));
    }
    // else if (key == "vn") {} -- for vertex normals eventually
    else if (key == "f") {
      std::string token;
      while (ss >> token) {
        std::replace(token.begin(), token.end(), '/', ' ');
        std::istringstream ts(token);

        unsigned int vertIndex, uvIndex;
        ts >> vertIndex >> uvIndex;

        if (indexMap.find({ vertIndex - 1, uvIndex - 1 }) != indexMap.end()) {
          indices.emplace_back(indexMap.at({ vertIndex - 1, uvIndex - 1 }));
          continue;
        }

        indexMap.emplace(std::make_pair(std::make_pair(vertIndex - 1, uvIndex - 1), vertices.size()));
        indices.emplace_back(vertices.size());

        vertices.emplace_back(hlvl::Vertex{ positions[vertIndex - 1], uvs[uvIndex - 1] });
      }
    }
  }

  return { std::move(vertices), std::move(indices) };
}

} // namespace obj