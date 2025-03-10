#pragma once

#include "src/core/include/context.hpp"
#include "src/core/include/settings.hpp"

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
      data = d;
      memcpy((char *)memoryMap + offsets[Context::frameIndex()], &data, sizeof(data));
    }

    void operator = (T&& d) {
      data = d;
      memcpy((char *)memoryMap + offsets[Context::frameIndex()], &data, sizeof(data));
    }

    const T& operator * (int) const {
      return data;
    }

  protected:
    void initialize() override {
      for (int i = 0; i < hlvl_settings.buffer_mode; ++i)
        memcpy((char *)memoryMap + offsets[i], &data, sizeof(data));
    }

  private:
    T data;
};

} // namespace