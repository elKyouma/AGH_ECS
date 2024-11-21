#pragma once
#include "Types.hpp"
#include "Component.hpp"
#include <array>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <memory>

class System
{
public:
    void Init(std::array<Signature, MAX_ENTITY_COUNT>& signatures,
              std::unordered_map<std::type_index, ComponentPoolId>& compToId)
    {
        SetSignature(compToId);
        ASSERT(systemSignature.to_ulong() != 0u);
        
        for(EntityId id = 0; id < MAX_ENTITY_COUNT; id++)
            if((systemSignature.to_ulong() & signatures[id].to_ulong()) == systemSignature.to_ulong())
                entities.emplace(id);
    }

    //TODO: Think about making update protected, and befriending ECS

    virtual void SetSignature(std::unordered_map<std::type_index, ComponentPoolId>& compToId) = 0;
    virtual void Update(std::unordered_map<std::type_index, ComponentPoolId>& typeToCompId, 
                        std::array<std::unique_ptr<IComponentPool>, MAX_COMPONENT_COUNT>& components) = 0;

    void OnEntityDestroyed(const EntityId entity)
    {
        ASSERT(entities.find(entity) != entities.end())
        entities.erase(entity);
    }

    void OnEntitySignatureChanged(const EntityId entity, const Signature newSignature)
    {
        if(entities.find(entity) == entities.end())
        {     
            if((systemSignature.to_ulong() & newSignature.to_ulong()) == systemSignature.to_ulong())
                entities.emplace(entity);
        }
        else 
            if((systemSignature.to_ulong() & newSignature.to_ulong()) != systemSignature.to_ulong())
                entities.erase(entity);
    }

    #ifdef IN_TEST
    bool CheckIfEntitySubscribed(const EntityId entity) 
    {
        return entities.find(entity) != entities.end();
    }
    #endif // IN_TEST
    
    virtual ~System(){};

protected:
    std::unordered_set<EntityId> entities;
    Signature systemSignature;
};
