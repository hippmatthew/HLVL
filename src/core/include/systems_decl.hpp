#pragma once

#include "components.hpp"

#include <set>

namespace hlvl
{

class ISystem
{
  friend class SystemManager;

  public:
    ISystem() = default;
    ISystem(ISystem&) = delete;
    ISystem(ISystem&&) = delete;

    virtual ~ISystem() = default;

    ISystem& operator = (ISystem&) = delete;
    ISystem& operator = (ISystem&&) = delete;

  protected:
    template <typename T>
    T& component(Entity);

  private:
    ComponentManager * p_componentManager = nullptr;

  protected:
    std::set<Entity> entities;
};

class SystemManager
{
  public:
    SystemManager() = default;
    SystemManager(SystemManager&) = delete;
    SystemManager(SystemManager&&) = delete;

    ~SystemManager() = default;

    SystemManager& operator = (SystemManager&) = delete;
    SystemManager& operator = (SystemManager&&) = delete;

    template <typename T>
    std::shared_ptr<T> system();

    template <typename T>
    Signature& signature();

    template <typename T>
    void addEntities(std::vector<Entity>&&);

    template <typename T>
    void removeEntities(std::vector<Entity>&&);

    template <typename... Tps>
    void addSystems(ComponentManager *);

    template <typename... Tps>
    void removeSystems();

  private:
    template <typename T>
    bool isRegistered() const;

    template <typename T>
    void addSystem(ComponentManager *);

    template <typename T>
    void removeSystem();

  private:
    std::map<const char *, std::shared_ptr<ISystem>> systemMap;
    std::map<const char *, Signature> signatureMap;
};

} // namespace hlvl