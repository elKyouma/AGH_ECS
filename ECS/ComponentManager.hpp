#pragma once

#include <array>
#include <memory>
#include <typeindex>
#include "Component.hpp"

class ComponentManager
{
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
        auto& comp = GetComponentPool<Component>();
        return comp.GetComponent(entity);
    }
    
    template <typename Component>
    const Component& GetComponent(const EntityId entity) const
    {
        const auto& comp = GetComponentPool<Component>(); 
        return comp.GetComponent(entity);
    }

    template <typename Component>
    Component& AddComponent(const EntityId entity)
    {
        auto& comp = GetComponentPool<Component>();
        return comp.AddComponent(entity);
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
        for(int i = 0; i < numberOfComponentPools; i++)
            components[i]->TryDeleteComponent(entity);
    }

private:   
    std::array<std::unique_ptr<IComponentPool>, MAX_COMPONENT_COUNT> components;
    ComponentPoolId numberOfComponentPools = 0;
    std::unordered_map<std::type_index, ComponentPoolId> typeToCompId;
};
