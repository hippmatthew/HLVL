#ifndef physp_core_resource_hpp
#define physp_core_resource_hpp

#include "src/core/include/resource_decl.hpp"

namespace pp
{

template <typename T>
Resource<T>::Resource(T d, ResourceType t, bool s) : IResource(sizeof(T))
{
  resource_type = t;
  is_shared = s;
  data = d;
}

template <typename T>
Resource<T>& Resource<T>::operator = (const Resource<T>& resource)
{
  if (this == &resource) return *this;

  data = resource.data;
  updateAllocation();

  return *this;
}

template <typename T>
Resource<T>& Resource<T>::operator = (Resource<T>&& resource)
{
  if (this == &resource) return *this;

  data = std::move(resource.data);
  updateAllocation();

  return *this;
}

template <typename T>
Resource<T>& Resource<T>::operator = (T& d)
{
  data = d;
  updateAllocation();

  return *this;
}

template <typename T>
Resource<T>& Resource<T>::operator = (T&& d)
{
  data = d;
  updateAllocation();

  return *this;
}

template <typename T>
const T& Resource<T>::operator * () const
{
  return data;
}

template <typename T>
const vk::raii::Buffer& Resource<T>::buffer() const
{
  if (p_buffer == nullptr)
    throw std::runtime_error("pp::Resource: tried to access buffer before being allocated");

  return p_buffer->buffer();
}

template <typename T>
void Resource<T>::updateAllocation() const
{
  if (p_allocator != nullptr)
    p_allocator->update_allocation(allocation_index, static_cast<void *>(const_cast<T *>(&data)), &data_mutex);
}

} // namespace pp

#endif // physp_core_resource_hpp