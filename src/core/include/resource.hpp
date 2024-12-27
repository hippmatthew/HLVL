#pragma once

#include "resource_decl.hpp"

namespace hlvl
{

template <typename T>
Resource<T>::Resource(T d, ResourceType t, bool s) : IResource(sizeof(T))
{
  resource_type = t;
  is_shared = s;
  data = d;
}

template <typename T>
Resource<T>::Resource(const Resource<T>& resource) : IResource(sizeof(T))
{
  resource_type = resource.resource_type;
  is_shared = resource.is_shared;
  data = resource.data;
}

template <typename T>
Resource<T>::Resource(Resource<T>&& resource) : IResource(sizeof(T))
{
  resource_type = std::move(resource.resource_type);
  is_shared = std::move(resource.is_shared);
  data = std::move(resource.data);
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
    throw std::runtime_error("hlvl::Resource: tried to access buffer before being allocated");

  return p_buffer->buffer();
}

template <typename T>
void Resource<T>::updateAllocation() const
{
  if (p_allocator != nullptr)
    p_allocator->update_allocation(allocation_index, static_cast<void *>(const_cast<T *>(&data)), &mutex);
}

} // namespace hlvl