#ifndef physp_core_resource_decl_hpp
#define physp_core_resource_decl_hpp

#ifndef physp_vulkan_include
#define physp_vulkan_include

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan_raii.hpp>

#endif // physp_vulkan_include

#include <initializer_list>
#include <vector>

namespace pp
{

enum ResourceType
{
  vertex_buffer,
  index_buffer,
  uniform,
  storage,
  texture
};

enum Locality
{
  host,
  device
};

class ResourcePool;

class IResource
{
  friend ResourcePool;

  public:
    IResource() = default;
    IResource(const IResource&);
    IResource(IResource&&);

    ~IResource() = default;

    IResource& operator = (const IResource&);
    IResource& operator = (IResource&&);

  public:
    vk::BufferUsageFlagBits usage;
    vk::SharingMode sharing_mode;

  protected:
    ResourcePool * p_allocation = nullptr;
    const unsigned long allocationIndex = -1;
};

template <typename T>
class Resource : public IResource
{
  public:
    Resource(T&, ResourceType, bool shared = false);
    Resource(T&&, ResourceType, bool shared = false);
    Resource(const Resource&) = default;
    Resource(Resource&&) = default;

    ~Resource() = default;

    Resource& operator = (const Resource&);
    Resource& operator = (Resource&&);
    Resource& operator = (T&);
    Resource& operator = (T&&);

  public:
    const unsigned long size = sizeof(T);

  private:
    T data;
};

class ResourcePool
{
  public:
    ResourcePool(std::vector<IResource *>, Locality l = device);
    ResourcePool(std::initializer_list<IResource *>);
    ResourcePool(const ResourcePool&) = delete;
    ResourcePool(ResourcePool&&);

    ~ResourcePool();

    ResourcePool& operator = (const ResourcePool&) = delete;
    ResourcePool& operator = (ResourcePool&&);
    ResourcePool& operator = (std::initializer_list<IResource *>);

    template <typename T>
    Resource<T>& operator [] (unsigned long);

    void clear();

  public:
    Locality locality = device;

  private:
    std::vector<IResource *> resources;
};

} // namespace pp

#endif // physp_core_resource_decl_hpp