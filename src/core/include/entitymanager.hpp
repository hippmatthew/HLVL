#ifndef physp_core_entitymanager_hpp
#define physp_core_entitymanager_hpp

#include "src/core/include/signature.hpp"

#include <map>

namespace pp
{

using Entity = unsigned long;

class EntityManager
{
  public:
    EntityManager() = default;
    EntityManager(EntityManager&) = delete;
    EntityManager(EntityManager&&) = delete;

    ~EntityManager() = default;

    EntityManager& operator = (EntityManager&) = delete;
    EntityManager& operator = (EntityManager&&) = delete;

    Entity newEntity();
    Signature& signature(Entity);

  private:
    void validateEntity(Entity);

  private:
    std::map<unsigned long, Signature> signatureMap;
    Entity nextEntity = 0;
};

} // namespace pp

#endif // physp_core_entitymanager_hpp