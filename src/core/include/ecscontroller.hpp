#ifndef physp_core_ecscontroller_hpp
#define physp_core_ecscontroller_hpp

#include "src/core/include/ecscontroller_decl.hpp"

namespace pp
{

template <typename T>
T& ECSController::component(Entity entity)
{
  return componentManager.component<T>(entity);
}

template <typename... Tps>
void ECSController::register_components()
{
  componentManager.newArrays<Tps...>();
}

template <typename... Tps>
void ECSController::unregister_components()
{
  componentManager.removeArrays<Tps...>();
}

template <typename... Tps>
void ECSController::add_components(Entity entity, Tps&... components)
{
  componentManager.addDatas(entity, components...);
  entityManager.signature(entity) |= ( componentManager.signature<Tps>(), ... );
}

template <typename... Tps>
void ECSController::add_components(Entity entity, Tps&&... components)
{
  componentManager.addDatas(entity, components...);
  entityManager.signature(entity) |= ( componentManager.signature<Tps>(), ... );
}

template <typename... Tps>
void ECSController::remove_components(Entity entity)
{
  componentManager.removeDatas<Tps...>(entity);
  entityManager.signature(entity) &= ( ~componentManager.signature<Tps>(), ... );
}

} // namespace pp

#endif // physp_core_ecscontroller_hpp