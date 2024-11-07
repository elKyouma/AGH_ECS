#pragma once
#include <array>
#include <cassert>
#include <stack>
#include <unordered_map>
#include "Types.hpp"

class IComponentPool
{
public:
    virtual ~IComponentPool();
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
        for(ComponentId id = MAX_ENTITY_COUNT - 1; id >= 0; id--)
            availableIds.push(id);
    }

    void AddComponent(const EntityId entity)
    {
        assert(entityToComponentId.find(entity) == entityToComponentId.end());

        entityToComponentId[entity] = availableIds.top();
        availableIds.pop();
    }
    
    Component& GetComponent(const EntityId entity) override
    {
        assert(components.find(entity) == components.end());
        return components[entity];   
    }
    
    const Component& GetComponent(const EntityId entity) const override
    { 
        assert(components.find(entity) == components.end());
         return components[entity];   
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

