#include "src/core/include/ecscontroller.hpp"

namespace pp
{

Entity ECSController::new_entity()
{
  return entityManager.newEntity();
}

} // namespace pp