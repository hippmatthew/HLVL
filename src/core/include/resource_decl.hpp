#ifndef physp_core_resource_decl_hpp
#define physp_core_resource_decl_hpp

#include <initializer_list>
#include <memory>
#include <vector>

namespace pp
{

class ResourcePool;

class IResource
{
  public:
    IResource() = default;
    IResource(const IResource&);
    IResource(IResource&&);

    virtual ~IResource() = default;

    IResource& operator = (const IResource&);
    IResource& operator = (IResource&&);

};

template <typename T>
class Resource : public IResource
{
  public:
    Resource(T&);
    Resource(T&&);
    Resource(const Resource&) = default;
    Resource(Resource&&) = default;

    ~Resource() = default;

    Resource& operator = (const Resource&) = default;
    Resource& operator = (Resource&&) = default;
    Resource& operator = (T&);
    Resource& operator = (T&&);

  public:
    T data;
};

class ResourcePool
{
  public:
    ResourcePool(std::initializer_list<std::shared_ptr<IResource>>);
    ResourcePool(const ResourcePool&) = default;
    ResourcePool(ResourcePool&&) = default;

    ~ResourcePool() = default;

    ResourcePool& operator = (const ResourcePool&) = default;
    ResourcePool& operator = (ResourcePool&&) = default;

  private:
    std::vector<std::shared_ptr<IResource>> resources;
};

} // namespace pp

#endif // physp_core_resource_decl_hpp