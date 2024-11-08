#pragma once
#include <array>
#include <cassert>
#include <functional>
#include <optional>
#include <stack>
#include <unordered_map>
#include "Types.hpp"

class IComponentPool
{
public:
    virtual bool TryDeleteComponent(const EntityId) = 0;
    virtual ~IComponentPool() {}
};

template <typename Component>
class ComponentPool : IComponentPool
{
private:
    //TODO: Think about supporting different types provided by the user
    //TODO: Think about invalid debug onlny value
    using ComponentId = uint32_t;

public:

    ComponentPool()
    {
        for(ComponentId id = MAX_ENTITY_COUNT; id > 0; id--)
            availableIds.push(id - 1);
    }

    Component& AddComponent(const EntityId entity)
    {
        assert(entityToComponentId.find(entity) == entityToComponentId.end());

        entityToComponentId[entity] = availableIds.top();
        availableIds.pop();
        return entityToComponentId[entity];
    }
    
    Component& GetComponent(const EntityId entity)
    {
        assert(components.find(entity) == components.end());
        return components[entity];   
    }
    
    const Component& GetComponent(const EntityId entity) const
    { 
        assert(components.find(entity) == components.end());
         return components[entity];   
    }
    
    std::optional<std::reference_wrapper<Component>> TryGetComponent(const EntityId entity)
    {
        if(entityToComponentId.find(entity) != components.end())
            return {components[entity]};
        else
            return {};
    }
    
    std::optional<std::reference_wrapper<const Component>> TryGetComponent(const EntityId entity) const
    { 
        if(entityToComponentId.find(entity) != components.end())
            return {components[entity]};
        else
            return {};
    }


    bool TryDeleteComponent(const EntityId entity) override
    {
        if(entityToComponentId.find(entity) == entityToComponentId.end())
            return false;

        availableIds.push(entity);
        entityToComponentId.erase(entity);

        return true;
    }

    void DeleteComponent(const EntityId entity)
    {
        assert(entityToComponentId.find(entity) != entityToComponentId.end());
        availableIds.push(entity);
        entityToComponentId.erase(entity);
    }
    
    const Component& operator[] (const EntityId entity) const;
    Component& operator[] (const EntityId entity);

private:
    std::array<Component, MAX_COMPONENT_COUNT> components;
    std::unordered_map<EntityId, ComponentId> entityToComponentId;
    std::stack<ComponentId> availableIds; 
    //TODO: check is it really a good idea    
};

