#ifndef physp_core_components_hpp
#define physp_core_components_hpp

#include "src/core/include/components_decl.hpp"

#include <stdexcept>

namespace pp
{

template <typename T>
T& ComponentArray<T>::operator [] (Entity entity)
{
  if (indexMap.find(entity) == indexMap.end())
    throw std::out_of_range("pp::ComponentArray: attempted to get component for entity not in array");

  return data[indexMap.at(entity)];
}

template <typename T>
void ComponentArray<T>::emplace(Entity entity, T& component)
{
  if (indexMap.find(entity) != indexMap.end())
  {
    data[indexMap.at(entity)] = component;
    return;
  }

  indexMap.emplace(std::pair(entity, data.size()));
  entityMap.emplace(std::pair(data.size(), entity));
  data.emplace_back(component);
}

template <typename T>
void ComponentArray<T>::emplace(Entity entity, T&& component)
{
  if (indexMap.find(entity) != indexMap.end())
  {
    data[indexMap.at(entity)] = std::move(component);
    return;
  }

  indexMap.emplace(std::pair(entity, data.size()));
  entityMap.emplace(std::pair(data.size(), entity));
  data.emplace_back(std::move(component));
}

template <typename T>
void ComponentArray<T>::erase(Entity entity)
{
  if (indexMap.find(entity) == indexMap.end())
    throw std::out_of_range("pp::ComponentArray: attempted to erase data for entity not in component array");

  entityMap.erase(indexMap.at(entity));

  for (unsigned long i = indexMap.at(entity) + 1; i < data.size(); ++i)
  {
    indexMap.at(entityMap.at(i)) = i - 1;

    auto entity = entityMap.at(i);
    entityMap.erase(i);
    entityMap.emplace(std::make_pair(i - 1, entity));
  }

  data.erase(data.begin() + indexMap.at(entity));
  indexMap.erase(entity);
}

template <typename T>
const Signature& ComponentManager::signature() const
{
  if (!isRegistered<T>())
    throw std::out_of_range("pp::ComponentManager: attempted to access signature of non-registered component type");

  return signatureMap.at(typeid(T).name());
}

template <typename T>
T& ComponentManager::component(Entity entity)
{
  return (*array<T>())[entity];
}

template <typename... Tps>
void ComponentManager::newArrays()
{
  ( newArray<Tps>(), ... );
}

template <typename... Tps>
void ComponentManager::removeArrays()
{
  ( removeArray<Tps>(), ... );
}

template <typename... Tps>
void ComponentManager::addDatas(Entity entity, Tps&... datas)
{
  ( addData<Tps>(entity, datas), ... );
}

template <typename... Tps>
void ComponentManager::addDatas(Entity entity, Tps&&... datas)
{
  ( addData<Tps>(entity, std::move(datas)), ... );
}

template <typename... Tps>
void ComponentManager::removeDatas(Entity entity)
{
  ( removeData<Tps>(entity), ... );
}

template <typename T>
bool ComponentManager::isRegistered() const
{
  return componentMap.find(typeid(T).name()) != componentMap.end();
}

template <typename T>
std::shared_ptr<ComponentArray<T>> ComponentManager::array()
{
  if (!isRegistered<T>())
    throw std::out_of_range("pp::ComponentManager: attempted to access component array of non-registered component type");

  return static_pointer_cast<ComponentArray<T>>(componentMap.at(typeid(T).name()));
}

template <typename T>
void ComponentManager::newArray()
{
  if (isRegistered<T>())
  {
    throw std::runtime_error(
      "pp::ComponentManager: attempted to add component array of previously registered component type"
    );
  }

  if (componentIndex > 63)
    throw std::runtime_error("pp::ComponentManager: attempted to add component array while at max capacity");

  componentMap.emplace(std::make_pair(typeid(T).name(), std::make_shared<ComponentArray<T>>()));
  signatureMap.emplace(std::make_pair(typeid(T).name(), Signature(1 << componentIndex++)));
}

template <typename T>
void ComponentManager::removeArray()
{
  if (!isRegistered<T>())
    throw std::out_of_range("pp::ComponentManager: attempted to remove component array of non-registered component type");

  componentMap.erase(typeid(T).name());
  signatureMap.erase(typeid(T).name());
}

template <typename T>
void ComponentManager::addData(Entity entity, T& data)
{
  array<T>()->emplace(entity, data);
}

template <typename T>
void ComponentManager::addData(Entity entity, T&& data)
{
  array<T>()->emplace(entity, std::move(data));
}

template <typename T>
void ComponentManager::removeData(Entity entity)
{
  array<T>()->erase(entity);
}

} // namespace pp

#endif // physp_core_components_hpp