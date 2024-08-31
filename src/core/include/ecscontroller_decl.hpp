#ifndef physp_core_ecscontroller_decl_hpp
#define physp_core_ecscontroller_decl_hpp

#include "src/core/include/entitymanager.hpp"
#include "src/core/include/components.hpp"
#include "src/core/include/systems.hpp"

namespace pp
{

class ECSController
{
  public:
    ECSController() = default;
    ECSController(ECSController&) = delete;
    ECSController(ECSController&&) = delete;

    ~ECSController() = default;

    ECSController& operator = (ECSController&) = delete;
    ECSController& operator = (ECSController&&) = delete;

    Entity new_entity();

    template <typename T>
    T& component(Entity);

    template <typename T>
    std::shared_ptr<T> system();

    template <typename T>
    ECSController& add_to_system(std::vector<Entity>&&);

    template <typename T>
    ECSController& remove_from_system(std::vector<Entity>&&);

    template <typename... Tps>
    ECSController& register_components();

    template <typename... Tps>
    ECSController& unregister_components();

    template <typename... Tps>
    ECSController& register_systems();

    template <typename... Tps>
    ECSController& unregister_systems();

    template <typename... Tps>
    ECSController& add_components(Entity, Tps&...);

    template <typename... Tps>
    ECSController& add_components(Entity, Tps&&...);

    template <typename ... Tps>
    ECSController& remove_components(Entity);

    template <typename System, typename... Components>
    ECSController& require_components();

    template <typename System, typename... Components>
    ECSController& unrequire_components();

  private:
    EntityManager entityManager;
    ComponentManager componentManager;
    SystemManager systemManager;
};

} // namespace pp

#endif // physp_core_ecscontroller_decl_hpp