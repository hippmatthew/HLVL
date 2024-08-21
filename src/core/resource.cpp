#include "src/core/include/resource_decl.hpp"

namespace pp
{

IResource::IResource(std::size_t s) : size(s) {}

IResource::IResource(const IResource& resource) : size(resource.size) {}

IResource::IResource(IResource&& resource) : size(resource.size) {}

IResource::~IResource()
{
  p_allocator = nullptr;
  p_buffer = nullptr;
}

IResource& IResource::operator = (const IResource&)
{
  return *this;
}

IResource& IResource::operator = (IResource&&)
{
  return *this;
}

} // namespace pp