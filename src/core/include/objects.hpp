#pragma once

#include "src/core/include/vertex.hpp"

namespace hlvl {

class Object {
  friend class Objects;

  private:
    class ObjectBuilder {
      friend class Object;
      friend class Objects;

      public:
        ObjectBuilder() = default;
        ObjectBuilder(ObjectBuilder&) = delete;
        ObjectBuilder(ObjectBuilder&&) = delete;

        ~ObjectBuilder() = default;

        ObjectBuilder& operator = (ObjectBuilder&) = delete;
        ObjectBuilder& operator = (ObjectBuilder&&) = delete;

        ObjectBuilder& add_vertices(std::vector<Vertex>);
        ObjectBuilder& add_indices(std::vector<unsigned int>);
        ObjectBuilder& add_material(std::string);

      private:
        std::string material;
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
    };

  public:
    Object() = delete;
    Object(Object&) = delete;
    Object(Object&&) = delete;
    Object(ObjectBuilder&);

    ~Object() = default;

    Object& operator = (Object&) = delete;
    Object& operator = (Object&&) = delete;

  private:
    vk::DeviceMemory vk_memory = nullptr;
    vk::Buffer vk_vBuffer = nullptr;
    vk::Buffer vk_iBuffer = nullptr;
    unsigned int iBufferOffset = 0;
};

class Objects {
  public:
    Objects(Objects&) = delete;
    Objects(Objects&&) = delete;

    Objects& operator = (Object&) = delete;
    Objects& operator = (Objects&&) = delete;

    static Objects& instance();
    static void destroy();

    void count() const;
    void add(Object::ObjectBuilder&);

  private:
    static Objects * p_objects;
    std::vector<Object> objects;

};

} // namespace hlvl