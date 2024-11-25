#pragma once
#include <array>
#include <optional>
#include <stack>
#include <unordered_map>
#include <vector>
#include "Types.hpp"

class IComponentPool
{
public:
    virtual bool TryDeleteComponent(const EntityId) = 0;
    virtual ~IComponentPool() {};
};

template <typename Component>
class ComponentPool : public IComponentPool
{
private:
    //TODO: Think about supporting different types provided by the user
    //TODO: Think about invalid debug onlny value

public:

    ComponentPool(ComponentId MAX_SIZE = MAX_ENTITY_COUNT)
    {
        components.resize(MAX_SIZE);
        for(ComponentId id = MAX_SIZE; id > 0; id--)
            availableIds.push(id - 1);
    }

    Component& AddComponent(const EntityId entity)
    {
        ASSERT(availableIds.size() > 0 && entityToComponentId.find(entity) == entityToComponentId.end());

        entityToComponentId[entity] = availableIds.top();
        availableIds.pop();
        return components[entityToComponentId[entity]];
    }
    
    Component& GetComponent(const EntityId entity)
    {
        ComponentId compId = entityToComponentId[entity];
        ASSERT(entityToComponentId.find(entity) != entityToComponentId.end());
        return components[compId];   
    }
    
    const Component& GetComponent(const EntityId entity) const
    { 
        ComponentId compId = entityToComponentId.at(entity);
        ASSERT(entityToComponentId.find(entity) != entityToComponentId.end());
        return components[compId];   
    }
    
    std::optional<std::reference_wrapper<Component>> TryGetComponent(const EntityId entity)
    {
        if(entityToComponentId.find(entity) != entityToComponentId.end())
            return {components[entity]};
        else
            return {};
    }
    
    std::optional<std::reference_wrapper<const Component>> TryGetComponent(const EntityId entity) const
    { 
        if(entityToComponentId.find(entity) != entityToComponentId.end())
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
        ASSERT(entityToComponentId.find(entity) != entityToComponentId.end());
        availableIds.push(entity);
        entityToComponentId.erase(entity);
    }
    
    const Component& operator[] (const EntityId entity) const;
    Component& operator[] (const EntityId entity);

private:
    std::vector<Component> components;
    std::unordered_map<EntityId, ComponentId> entityToComponentId;
    std::stack<ComponentId> availableIds; 
    //TODO: check is it really a good idea    
};

