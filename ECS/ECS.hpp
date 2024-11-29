#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <span>
#include <stack>
#include <typeindex>
#include <unordered_map>
#include <utility>

#include "ComponentManager.hpp"
#include "Types.hpp"
#include "System.hpp"

class ECS
{
public:
    template <typename System, typename... ARGS>
    void RegisterSystem(ARGS... args)
    {
        ASSERT(typeToSysId.find(std::type_index(typeid(System))) == typeToSysId.end());
        
        typeToSysId[std::type_index(typeid(System))] = numberOfSystems;
        systems[numberOfSystems] = std::make_unique<System>(std::forward<ARGS>(args) ...);
        systems[numberOfSystems]->Init(signatures, &compManager);
        numberOfSystems++;
    }

    template <typename Component>
    void RegisterComponentPool(ComponentId MAX_SIZE = MAX_ENTITY_COUNT)
    {
        compManager.RegisterComponentPool<Component>(MAX_SIZE);
    }

    template <typename Component>
    Component& GetComponent(const EntityId entity)
    {
        ASSERT(signatures[entity].test(compManager.CompId<Component>()));
        return compManager.GetComponent<Component>(entity);
    }
    
    template <typename Component>
    const Component& GetComponent(const EntityId entity) const
    {
        ASSERT(signatures[entity].test(compManager.CompId<Component>()));
        return compManager.GetComponent<Component>(entity);
    }

    template <typename Component>
    std::optional<std::reference_wrapper<Component>> TryGetComponent(const EntityId entity)
    {
        return compManager.TryGetComponent<Component>(entity);
    }
    
    template <typename Component>
    std::optional<std::reference_wrapper<const Component>> TryGetComponent(const EntityId entity) const
    {
        return compManager.TryGetComponent<Component>(entity);
    }

    template <typename Component, typename... ARGS>
    Component& AddComponent(const EntityId entity, ARGS&&... args)
    {
        auto& comp = compManager.AddComponent<Component>(entity, args...);
        signatures[entity].set(compManager.CompId<Component>());
        for(SystemId sysId = 0; sysId < numberOfSystems; sysId++)
            systems[sysId]->OnEntitySignatureChanged(entity, signatures[entity]);
        return comp;
    }
    
    template <typename Component>
    void DeleteComponent(const EntityId entity)
    {
        ASSERT(signatures[entity].test(compManager.CompId<Component>()));
        signatures[entity].reset(compManager.CompId<Component>());
        for(SystemId sysId = 0; sysId < numberOfSystems; sysId++)
            systems[sysId]->OnEntitySignatureChanged(entity, signatures[entity]);
        
        compManager.DeleteComponent<Component>(entity);       
    }

    template <typename Component>
    void TryDeleteComponent(const EntityId entity)
    {
        ASSERT(signatures[entity].test(compManager.CompId<Component>()));
        signatures[entity].reset(compManager.CompId<Component>());
        for(SystemId sysId = 0; sysId < numberOfSystems; sysId++)
            systems[sysId]->OnEntitySignatureChanged(entity, signatures[entity]);
        
        compManager.TryDeleteComponent<Component>(entity);       
    }

    template<typename Component, typename... ARGS>
    void AddComponents(std::span<EntityId> entities, ARGS&&... args)
    {
        for(const auto ent : entities)
        {    
            signatures[ent].set(compManager.CompId<Component>());
            for(SystemId sysId = 0; sysId < numberOfSystems; sysId++)
                systems[sysId]->OnEntitySignatureChanged(ent, signatures[ent]);
        }
        compManager.AddComponents<Component>(entities, args...);
    }

    template<typename Component>
    void DeleteComponents(std::span<EntityId> entities)
    { 
        for(const auto ent : entities)
        {   
            signatures[ent].reset(compManager.CompId<Component>());
            for(SystemId sysId = 0; sysId < numberOfSystems; sysId++)
                systems[sysId]->OnEntitySignatureChanged(ent, signatures[ent]);
        }

        compManager.DeleteComponents<Component>(entities);
    }

    void DestroyEntities(std::span<EntityId> entities)
    {
        for(const auto ent : entities)
            DestroyEntity(ent);
    }

    ECS();
    ~ECS();

    EntityId CreateEntity();
    void DestroyEntity(const EntityId entity);
    void UpdateSystems(const float deltaTime);
    void RenderSystems();

private:
    std::array<std::unique_ptr<System>, MAX_SYSTEM_COUNT> systems;
    std::unordered_map<std::type_index, SystemId> typeToSysId;
    SystemId numberOfSystems = 0;
   
    ComponentManager compManager{};

    std::stack<EntityId> availableEntityIds;
    Signature* signatures;
};
