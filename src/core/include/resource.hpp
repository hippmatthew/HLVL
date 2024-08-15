#ifndef physp_core_resource_hpp
#define physp_core_resource_hpp

#include "src/core/include/resource_decl.hpp"
#include <stdexcept>
#include <vulkan/vulkan_enums.hpp>

namespace pp
{

template <typename T>
Resource<T>::Resource(T& initData, ResourceType t, bool shared)
{
  data = initData;
  sharing_mode = shared ? vk::SharingMode::eConcurrent : vk::SharingMode::eExclusive;

  switch(t)
  {
    case vertex_buffer:
      usage = vk::BufferUsageFlagBits::eVertexBuffer;
      break;
    case index_buffer:
      usage = vk::BufferUsageFlagBits::eIndexBuffer;
      break;
    case uniform:
      usage = vk::BufferUsageFlagBits::eUniformBuffer;
      break;
    case storage:
      usage = vk::BufferUsageFlagBits::eStorageBuffer;
      break;
  }
}

template <typename T>
Resource<T>& Resource<T>::operator = (const Resource<T>& resource)
{
  if (this == &resource) return *this;

  data = resource.data;
}

template <typename T>
Resource<T>& Resource<T>::operator = (T& newData)
{
  data = newData;
  return *this;
}

template <typename T>
Resource<T>& Resource<T>::operator = (T&& newData)
{
  data = std::move(newData);
  return *this;
}

template <typename T>
Resource<T>& ResourcePool::operator [] (unsigned long index)
{
  if (index >= resources.size())
    throw std::out_of_range("pp::ResourcePool: index out of range");

  if (resources[index] == nullptr)
    throw std::runtime_error("pp::ResourcePool: accessed invalid data");

  return *static_cast<Resource<T> *>(resources[index]);
}

} // namespace pp

#endif // physp_core_resource_hpp