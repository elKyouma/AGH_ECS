#include "Types.hpp"
#include "Component.hpp"
#include <memory>
#include <stack>
#include <typeindex>
#include <unordered_map>


class ECS
{
private:
    using ComponentPoolId = uint32_t;
    
    template<typename Component>
    ComponentPool<Component>* GetComponentPool()
    {
        return dynamic_cast<ComponentPool<Component>*>
            (components[typeToId[std::type_index(typeid(Component))]].get());
    }

public:
    ECS()
    {
       for(EntityId id = MAX_ENTITY_COUNT - 1; id >=0; id--)
            availableEntityIds.push(id);
    }

    EntityId CreateEntity()
    {
        auto newEntity = availableEntityIds.top();
        availableEntityIds.pop();
        return newEntity;
    }

    void DestroyEntity(EntityId entity)
    {
        for(auto& iComp : components)
            iComp->TryDeleteComponent(entity);

        availableEntityIds.push(entity);
    }

    template <typename Component>
    void RegisterComponentPool()
    {
        typeToId[std::type_index(typeid(Component))] = numberOfComponentPools;
        components[numberOfComponentPools] = std::make_unique<ComponentPool<Component>()>();
        numberOfComponentPools++;
    }

    template <typename Component>
    Component& GetComponent(const EntityId entity)
    {
        auto comp = GetComponentPool<Component>();
        return comp->GetComponent(entity);
    }
    
    template <typename Component>
    const Component& GetComponent(const EntityId entity) const
    {
        const auto comp = GetComponentPool<Component>(); 
        return comp->GetComponent(entity);
    }

    template <typename Component>
    Component& AddComponent(const EntityId entity)
    {
        auto comp = GetComponentPool<Component>();
        return comp->AddComponent(entity);
    }
    
    template <typename Component>
    void DeleteComponent(const EntityId entity)
    {
        auto comp = GetComponentPool<Component>();
        comp->DeleteComponent(entity);       
    }

    template <typename Component>
    void TryDeleteComponent(const EntityId entity)
    {
        auto comp = GetComponentPool<Component>();
        comp->TryDeleteComponent(entity);       
    }

private:
    std::array<std::unique_ptr<IComponentPool>, MAX_ENTITY_COUNT> components;
    std::stack<EntityId> availableEntityIds;
    std::unordered_map<std::type_index, ComponentPoolId> typeToId;
    std::array<Signature, MAX_ENTITY_COUNT> signatures;
    ComponentPoolId numberOfComponentPools = 0;
};
