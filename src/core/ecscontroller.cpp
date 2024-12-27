#include "include/ecscontroller.hpp"

namespace hlvl
{

Entity ECSController::new_entity()
{
  return entityManager.newEntity();
}

} // namespace hlvl