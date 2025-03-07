#pragma once

#include "src/core/include/vertex.hpp"

#define hlvl_objects hlvl::Objects::instance()

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
        std::string material = "";
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
    };

  public:
    Object() = delete;
    Object(Object&) = delete;
    Object(Object&&) = default;
    Object(ObjectBuilder&);

    ~Object() = default;

    Object& operator = (Object&) = delete;
    Object& operator = (Object&&) = default;

    static ObjectBuilder builder();

    #ifdef hlvl_tests

      const vk::raii::DeviceMemory& get_memory() const { return vk_memory; }
      const std::vector<vk::raii::Buffer>& get_buffers() const { return vk_buffers; }

    #endif // hlvl_tests

  private:
    std::string materialTag = "";
    vk::raii::DeviceMemory vk_memory = nullptr;
    std::vector<vk::raii::Buffer> vk_buffers;
};

class Objects {
  public:
    Objects(Objects&) = delete;
    Objects(Objects&&) = delete;

    Objects& operator = (Object&) = delete;
    Objects& operator = (Objects&&) = delete;

    static Objects& instance();
    static void destroy();

    unsigned int count() const;
    void add(Object::ObjectBuilder&);

    #ifdef hlvl_tests

      const Object& get_object(unsigned int index) const { return objects[index]; }

    #endif // hlvl_tests

  private:
    Objects() = default;
    ~Objects() = default;

  private:
    static Objects * p_objects;
    std::vector<Object> objects;
};

} // namespace hlvl