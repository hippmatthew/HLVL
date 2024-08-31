#ifndef physp_core_ecscontroller_hpp
#define physp_core_ecscontroller_hpp

#include "src/core/include/ecscontroller_decl.hpp"

#include <stdexcept>

namespace pp
{

template <typename T>
T& ECSController::component(Entity entity)
{
  return componentManager.component<T>(entity);
}

template <typename T>
std::shared_ptr<T> ECSController::system()
{
  return systemManager.system<T>();
}

template <typename T>
ECSController& ECSController::add_to_system(std::vector<Entity>&& entities)
{
  for (const auto& entity : entities)
  {
    if ((entityManager.signature(entity) & systemManager.signature<T>()) != systemManager.signature<T>())
      throw std::runtime_error("pp::ECSController: attempted to add entity of non-matching signature to system");
  }

  systemManager.addEntities<T>(std::move(entities));
  return *this;
}

template <typename T>
ECSController& ECSController::remove_from_system(std::vector<Entity>&& entities)
{
  systemManager.removeEntities<T>(std::move(entities));
  return *this;
}

template <typename... Tps>
ECSController& ECSController::register_components()
{
  componentManager.newArrays<Tps...>();
  return *this;
}

template <typename... Tps>
ECSController& ECSController::unregister_components()
{
  componentManager.removeArrays<Tps...>();
  return *this;
}

template <typename... Tps>
ECSController& ECSController::register_systems()
{
  systemManager.addSystems<Tps...>(const_cast<ComponentManager *>(&componentManager));
  return *this;
}

template <typename... Tps>
ECSController& ECSController::unregister_systems()
{
  systemManager.removeSystems<Tps...>();
  return *this;
}

template <typename... Tps>
ECSController&  ECSController::add_components(Entity entity, Tps&... components)
{
  componentManager.addDatas(entity, components...);
  entityManager.signature(entity) |= ( ... | componentManager.signature<Tps>() );
  return *this;
}

template <typename... Tps>
ECSController&  ECSController::add_components(Entity entity, Tps&&... components)
{
  componentManager.addDatas(entity, components...);
  entityManager.signature(entity) |= ( ... | componentManager.signature<Tps>() );
  return *this;
}

template <typename... Tps>
ECSController&  ECSController::remove_components(Entity entity)
{
  componentManager.removeDatas<Tps...>(entity);
  entityManager.signature(entity) &= (... & ~(componentManager.signature<Tps>()));
  return *this;
}

template <typename System, typename... Components>
ECSController& ECSController::require_components()
{
  systemManager.signature<System>() |= ( ... | componentManager.signature<Components>() );
  return *this;
}

template <typename System, typename... Components>
ECSController& ECSController::unrequire_components()
{
  systemManager.signature<System>() &= ( ... & componentManager.signature<Components>() );
  return *this;
}

} // namespace pp

#endif // physp_core_ecscontroller_hpp