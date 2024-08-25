#ifndef physp_core_components_decl_hpp
#define physp_core_components_decl_hpp

#include "src/core/include/entitymanager.hpp"

#include <map>
#include <vector>

namespace pp
{

class IComponentArray
{
  public:
    IComponentArray() = default;
    IComponentArray(IComponentArray&) = delete;
    IComponentArray(IComponentArray&&) = delete;

    virtual ~IComponentArray() = default;

    IComponentArray& operator = (IComponentArray&) = delete;
    IComponentArray& operator = (IComponentArray&&) = delete;
};

template <typename T>
class ComponentArray : public IComponentArray
{
  public:
    ComponentArray() = default;
    ~ComponentArray() = default;

    T& operator [] (Entity);

    void emplace(Entity, T&);
    void emplace(Entity, T&&);
    void erase(Entity);

  private:
    std::map<Entity, unsigned long> indexMap;
    std::map<unsigned long, Entity> entityMap;
    std::vector<T> data;
};

class ComponentManager
{
  public:
    ComponentManager() = default;
    ComponentManager(ComponentManager&) = delete;
    ComponentManager(ComponentManager&&) = delete;

    ~ComponentManager() = default;

    ComponentManager& operator = (ComponentManager&) = delete;
    ComponentManager& operator = (ComponentManager&&) = delete;

    template <typename T>
    const Signature& signature() const;

    template <typename T>
    T& component(Entity);

    template <typename... Tps>
    void newArrays();

    template <typename... Tps>
    void removeArrays();

    template <typename... Tps>
    void addDatas(Entity, Tps&...);

    template <typename... Tps>
    void addDatas(Entity, Tps&&...);

    template <typename... Tps>
    void removeDatas(Entity);

  private:
    template <typename T>
    bool isRegistered() const;

    template <typename T>
    std::shared_ptr<ComponentArray<T>> array();

    template <typename T>
    void newArray();

    template <typename T>
    void removeArray();

    template <typename T>
    void addData(Entity, T&);

    template <typename T>
    void addData(Entity, T&&);

    template <typename T>
    void removeData(Entity);

  private:
    std::map<const char *, std::shared_ptr<IComponentArray>> componentMap;
    std::map<const char *, Signature> signatureMap;
    unsigned long componentIndex = 0;
};

} // namespace pp

#endif // physp_core_components_decl_hpp