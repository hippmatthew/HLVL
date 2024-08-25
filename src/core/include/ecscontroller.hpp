#ifndef physp_core_ecscontroller_hpp
#define physp_core_ecscontroller_hpp

#include "src/core/include/entitymanager.hpp"

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

  private:
    EntityManager entityManager;
};

} // namespace pp

#endif // physp_core_ecscontroller_hpp