#pragma once
#include <memory>
#include <stack>
#include <typeindex>
#include <unordered_map>

#include "ComponentManager.hpp"
#include "Types.hpp"
#include "System.hpp"

class ECS
{
public:
    template <typename System>
    void RegisterSystem()
    {
        ASSERT(typeToSysId.find(std::type_index(typeid(System))) == typeToSysId.end());
        
        typeToSysId[std::type_index(typeid(System))] = numberOfSystems;
        systems[numberOfSystems] = std::make_unique<System>();
        systems[numberOfSystems]->Init(signatures, &compManager);
        numberOfSystems++;
    }

    template <typename Component>
    void RegisterComponentPool()
    {
        compManager.RegisterComponentPool<Component>();
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
    Component& AddComponent(const EntityId entity)
    {
        auto& comp = compManager.AddComponent<Component>(entity);
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

    ECS();

    EntityId CreateEntity();
    void DestroyEntity(const EntityId entity);
    void UpdateSystems();

private:
    std::array<std::unique_ptr<System>, MAX_SYSTEM_COUNT> systems;
    std::unordered_map<std::type_index, SystemId> typeToSysId;
    SystemId numberOfSystems = 0;
   
    ComponentManager compManager{};

    std::stack<EntityId> availableEntityIds;
    std::array<Signature, MAX_ENTITY_COUNT> signatures;
};
