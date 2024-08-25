#ifndef physp_core_ecscontroller_decl_hpp
#define physp_core_ecscontroller_decl_hpp

#include "src/core/include/entitymanager.hpp"
#include "src/core/include/components.hpp"

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

    template <typename... Tps>
    void register_components();

    template <typename... Tps>
    void unregister_components();

    template <typename... Tps>
    void add_components(Entity, Tps&...);

    template <typename... Tps>
    void add_components(Entity, Tps&&...);

    template <typename ... Tps>
    void remove_components(Entity);

  private:
    EntityManager entityManager;
    ComponentManager componentManager;
};

} // namespace pp

#endif // physp_core_ecscontroller_decl_hpp