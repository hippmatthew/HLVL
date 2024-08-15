#include "src/core/include/resource.hpp"

namespace pp
{

IResource::IResource(const IResource&) {}

IResource::IResource(IResource&&) {}

IResource& IResource::operator = (const IResource&)
{
  return *this;
}

IResource& IResource::operator = (IResource&&)
{
  return *this;
}

ResourcePool::ResourcePool(std::vector<IResource *> list, Locality l)
{
  locality = l;
  for (auto * resource : list)
    resources.emplace_back(resource);
}

ResourcePool::ResourcePool(std::initializer_list<IResource *> list)
{
  for (auto * resource : list)
    resources.emplace_back(resource);
}

ResourcePool::ResourcePool(ResourcePool&& pool)
{
  for (auto * resource : pool.resources)
    resources.emplace_back(resource);

  pool.clear();
}

ResourcePool::~ResourcePool()
{
  clear();
}

ResourcePool& ResourcePool::operator = (ResourcePool&& pool)
{
  if (this == &pool) return *this;

  clear();

  for (auto * resource : pool.resources)
    resources.emplace_back(resource);

  pool.clear();

  return *this;
}

ResourcePool& ResourcePool::operator = (std::initializer_list<IResource *> list)
{
  clear();

  for (auto * resource : list)
    resources.emplace_back(resource);

  return *this;
}

void ResourcePool::clear()
{
  for (auto * resource : resources)
    resource = nullptr;
}

} // namespace pp