#pragma once
#include <array>
#include <memory>
#include <span>
#include <typeindex>
#include "Component.hpp"
#include "Types.hpp"

class ComponentManager
{    
    using ComponentPoolId = uint16_t;

public:
    template<typename Component>
    constexpr ComponentPoolId CompId()
    {
        ASSERT(typeToCompId.find(std::type_index(typeid(Component))) != typeToCompId.end());
        return typeToCompId[std::type_index(typeid(Component))];
    }

    template<typename Component>
    ComponentPool<Component>& GetComponentPool()
    {
        return *dynamic_cast<ComponentPool<Component>*>
            (components[CompId<Component>()].get());
    }
    
    template<typename Component>
    const ComponentPool<Component>& GetComponentPool() const
    {
        return *dynamic_cast<ComponentPool<Component>*>
            (components[CompId<Component>()].get());
    }
    
    template <typename Component>
    void RegisterComponentPool(ComponentId MAX_SIZE = MAX_ENTITY_COUNT)
    {
        ASSERT(typeToCompId.find(std::type_index(typeid(Component))) == typeToCompId.end());
        typeToCompId[std::type_index(typeid(Component))] = numberOfComponentPools;
        components[numberOfComponentPools] = std::make_unique<ComponentPool<Component>>(MAX_SIZE);
        numberOfComponentPools++;
    }

    template <typename Component>
    Component& GetComponent(const EntityId entity)
    {
        auto& comp = GetComponentPool<Component>();
        return comp.GetComponent(entity);
    }
    
    template <typename Component>
    const Component& GetComponent(const EntityId entity) const
    {
        const auto& comp = GetComponentPool<Component>(); 
        return comp.GetComponent(entity);
    }

    template <typename Component, typename... ARGS>
    Component& AddComponent(const EntityId entity, ARGS&&... args)
    {
        auto& compPool = GetComponentPool<Component>();
        auto& comp = compPool.AddComponent(entity);
        comp = Component(std::forward<ARGS>(args)...);
        return comp;
    }
    
    template <typename Component>
    void DeleteComponent(const EntityId entity)
    {
        auto& comp = GetComponentPool<Component>();
        comp.DeleteComponent(entity);       
    }

    template <typename Component>
    void TryDeleteComponent(const EntityId entity)
    {
        auto& comp = GetComponentPool<Component>();
        comp.TryDeleteComponent(entity);       
    }

    void DestroyAllComponents(const EntityId entity)
    {
        for(ComponentPoolId i = 0; i < numberOfComponentPools; i++)
            components[i]->TryDeleteComponent(entity);
    }

    template<typename Component, typename... ARGS>
    void AddComponents(std::span<EntityId> entities, ARGS&&... args)
    {
        for(const auto ent : entities)
            AddComponent<Component>(ent, args...);
    }

    template<typename Component>
    void DeleteComponents(std::span<EntityId> entities)
    {
        for(const auto ent : entities)
            DeleteComponent<Component>(ent);
    }

private:   
    std::array<std::unique_ptr<IComponentPool>, MAX_COMPONENT_COUNT> components;
    ComponentPoolId numberOfComponentPools = 0;
    std::unordered_map<std::type_index, ComponentPoolId> typeToCompId;
};
