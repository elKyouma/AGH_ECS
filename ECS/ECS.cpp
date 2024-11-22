#include "System.hpp"
#include "Types.hpp"
#include "Component.hpp"
#include <memory>
#include <stack>
#include <typeindex>
#include <unordered_map>


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
    ECS()
    {
       for(EntityId id = MAX_ENTITY_COUNT; id > 0; id--)
            availableEntityIds.push(id - 1);
    }

    EntityId CreateEntity()
    {
        auto newEntity = availableEntityIds.top();
        availableEntityIds.pop();
        return newEntity;
    }

    void DestroyEntity(EntityId entity)
    {
        for(ComponentPoolId compId = 0; compId < numberOfComponentPools; compId++)
            components[compId]->TryDeleteComponent(entity);

        signatures[entity] = 0u;
        availableEntityIds.push(entity);
    }
    
    void UpdateSystems()
    {
        for(SystemId id = 0; id < numberOfSystems; id++)
            systems[id].get()->Update(typeToCompId, components);
    }

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
