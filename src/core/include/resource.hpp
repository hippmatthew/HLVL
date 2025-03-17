#pragma once

#include "src/core/include/context.hpp"

#include <vector>

namespace hlvl {

class ResourceProxy {
  friend class Material;

  public:
    ResourceProxy() = default;
    ResourceProxy(ResourceProxy&) = delete;
    ResourceProxy(ResourceProxy&&) = default;

    virtual ~ResourceProxy() = default;

    ResourceProxy& operator = (ResourceProxy&) = delete;
    ResourceProxy& operator = (ResourceProxy&&) = default;

  protected:
    virtual void initialize() = 0;

  protected:
    unsigned int size = 0;
    std::vector<unsigned int> offsets;
    void * memoryMap = nullptr;
};

template <typename T>
class Resource : public ResourceProxy {
  public:
    Resource(T& d) {
      data = d;
      size = sizeof(T);
    }

    Resource(T&& d) {
      data = d;
      size = sizeof(T);
    }

    Resource(Resource&&) = default;

    ~Resource() = default;

    Resource& operator = (Resource&&) = default;

    void operator = (T& d) {
      if (memoryMap == nullptr) {
        throw std::runtime_error(
          "hlvl: tried to assign data to either an uninitialized resource or a storage buffer"
        );
      }

      data = d;
      memcpy((char *)memoryMap + offsets[Context::frameIndex()], &data, sizeof(data));
    }

    void operator = (T&& d) {
      if (memoryMap == nullptr) {
        throw std::runtime_error(
          "hlvl: tried to assign data to either a non-initialized resource or a storage buffer"
        );
      }

      data = d;
      memcpy((char *)memoryMap + offsets[Context::frameIndex()], &data, sizeof(data));
    }

    const T& operator * () const {
      return data;
    }

  protected:
    void initialize() override {
      for (auto& offset : offsets)
        memcpy((char *)memoryMap + offset, &data, sizeof(data));
    }

  private:
    T data;
};

} // namespace