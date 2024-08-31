#ifndef physp_core_systems_hpp
#define physp_core_systems_hpp

#include "src/core/include/systems_decl.hpp"
#include "src/core/include/components.hpp"

#include <stdexcept>
#include <type_traits>

namespace pp
{

template <typename T>
T& ISystem::component(Entity entity)
{
  return p_componentManager->component<T>(entity);
}

template <typename T>
std::shared_ptr<T> SystemManager::system()
{
  if (!isRegistered<T>())
    throw std::out_of_range("pp::SystemManager: attempted to access an unregistered system");

  return static_pointer_cast<T>(systemMap.at(typeid(T).name()));
}

template <typename T>
Signature& SystemManager::signature()
{
  if (!isRegistered<T>())
    throw std::out_of_range("pp::SystemManager: attempted to access signature of unregistered system");

  return signatureMap.at(typeid(T).name());
}

template <typename T>
void SystemManager::addEntities(std::vector<Entity>&& entities)
{
  std::set<Entity>& systemEntities = system<T>()->entities;

  for (const auto& entity : entities)
  {
    if (systemEntities.contains(entity)) continue;
    systemEntities.emplace(entity);
  }
}

template <typename T>
void SystemManager::removeEntities(std::vector<Entity>&& entities)
{
  std::set<Entity>& systemEntities = system<T>()->entities;

  for (const auto& entity : entities)
  {
    if (!systemEntities.contains(entity)) continue;
    systemEntities.erase(entity);
  }
}

template <typename... Tps>
void SystemManager::addSystems(ComponentManager * p_componentManager)
{
  ( addSystem<Tps>(p_componentManager), ... );
}

template <typename... Tps>
void SystemManager::removeSystems()
{
  ( removeSystem<Tps>(), ... );
}

template <typename T>
bool SystemManager::isRegistered() const
{
  static_assert(std::is_base_of<ISystem, T>::value,
    "pp::SystemManager: system type must be derived from pp::ISystem"
  );

  return systemMap.find(typeid(T).name()) != systemMap.end();
}

template <typename T>
void SystemManager::addSystem(ComponentManager * p_componentManager)
{
  if (isRegistered<T>())
    throw std::runtime_error("pp::SystemManager: attempted to register previously registered system");

  systemMap.emplace(std::make_pair(typeid(T).name(), std::make_shared<T>()));
  signatureMap.emplace(std::make_pair(typeid(T).name(), Signature()));

  systemMap.at(typeid(T).name())->p_componentManager = p_componentManager;
}

template <typename T>
void SystemManager::removeSystem()
{
  if (!isRegistered<T>())
    throw std::out_of_range("pp::SystemManager: attempted to remove a non-registered system");

  systemMap.at(typeid(T).name())->p_componentManager = nullptr;

  systemMap.erase(typeid(T).name());
  signatureMap.erase(typeid(T).name());
}

} // namespace pp

#endif // physp_core_systems_hpp