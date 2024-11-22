#pragma once
#include <memory>
#include <stack>
#include <typeindex>
#include <unordered_map>

#include "Component.hpp"
#include "Types.hpp"
#include "System.hpp"

class ECS
{
private:
    template<typename Component>
    constexpr ComponentPoolId CompId()
    {
        ASSERT(typeToCompId.find(std::type_index(typeid(Component))) != typeToCompId.end());
        return typeToCompId[std::type_index(typeid(Component))];
    }

    template<typename System>
    constexpr ComponentPoolId SysId()
    {
        ASSERT(typeToSysId.find(std::type_index(typeid(System))) == typeToSysId.end());
        return typeToSysId[std::type_index(typeid(System))];
    }
    
    template<typename Component>
    ComponentPool<Component>* GetComponentPool()
    {
        return dynamic_cast<ComponentPool<Component>*>
            (components[CompId<Component>()].get());
    }

public:
    template <typename System>
    void RegisterSystem()
    {
        SysId<System>();
        typeToSysId[std::type_index(typeid(System))] = numberOfSystems;
        systems[numberOfSystems] = std::make_unique<System>();
        systems[numberOfSystems]->Init(signatures, typeToCompId);
        numberOfSystems++;
    }

    template <typename Component>
    void RegisterComponentPool()
    {
        ASSERT(typeToCompId.find(std::type_index(typeid(Component))) == typeToCompId.end());
        typeToCompId[std::type_index(typeid(Component))] = numberOfComponentPools;
        components[numberOfComponentPools] = std::make_unique<ComponentPool<Component>>();
        numberOfComponentPools++;
    }

    template <typename Component>
    Component& GetComponent(const EntityId entity)
    {
        ASSERT(signatures[entity].test(CompId<Component>()));
        auto comp = GetComponentPool<Component>();
        return comp->GetComponent(entity);
    }
    
    template <typename Component>
    const Component& GetComponent(const EntityId entity) const
    {
        ASSERT(signatures[entity].test(CompId<Component>()));
        const auto comp = GetComponentPool<Component>(); 
        return comp->GetComponent(entity);
    }

    template <typename Component>
    Component& AddComponent(const EntityId entity)
    {
        auto comp = GetComponentPool<Component>();
        signatures[entity].set(CompId<Component>());
        for(SystemId sysId = 0; sysId < numberOfSystems; sysId++)
            systems[sysId]->OnEntitySignatureChanged(entity, signatures[entity]);
        return comp->AddComponent(entity);
    }
    
    template <typename Component>
    void DeleteComponent(const EntityId entity)
    {
        ASSERT(signatures[entity].test(CompId<Component>()));
        auto comp = GetComponentPool<Component>();
        signatures[entity].reset(CompId<Component>());
        comp->DeleteComponent(entity);       
    }

    template <typename Component>
    void TryDeleteComponent(const EntityId entity)
    {
        ASSERT(signatures[entity].test(CompId<Component>()));
        auto comp = GetComponentPool<Component>();
        signatures[entity].reset(CompId<Component>());
        comp->TryDeleteComponent(entity);       
    }

    ECS();

    EntityId CreateEntity();
    void DestroyEntity(const EntityId entity);
    void UpdateSystems();

private:
    std::array<std::unique_ptr<IComponentPool>, MAX_COMPONENT_COUNT> components;
    std::array<std::unique_ptr<System>, MAX_SYSTEM_COUNT> systems;

    std::stack<EntityId> availableEntityIds;
    
    std::unordered_map<std::type_index, ComponentPoolId> typeToCompId;
    std::unordered_map<std::type_index, SystemId> typeToSysId;
    
    std::array<Signature, MAX_ENTITY_COUNT> signatures;
    ComponentPoolId numberOfComponentPools = 0;
    ComponentPoolId numberOfSystems = 0;


};
