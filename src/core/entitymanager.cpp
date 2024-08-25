#include "src/core/include/entitymanager.hpp"

#include <stdexcept>

namespace pp
{

Entity EntityManager::newEntity()
{
  signatureMap.emplace(std::make_pair(nextEntity, Signature()));
  return nextEntity++;
}

Signature& EntityManager::signature(Entity entity)
{
  validateEntity(entity);

  return signatureMap.at(entity);
}

void EntityManager::validateEntity(Entity entity)
{
  if (signatureMap.find(entity) != signatureMap.end()) return;

  throw std::out_of_range("pp::EntityManager: entity doesnt exist in the entity manager");
}

} // namespace pp