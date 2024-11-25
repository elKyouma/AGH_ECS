#include "ECS.hpp"

ECS::ECS()
{
    for(EntityId id = MAX_ENTITY_COUNT; id > 0; id--)
        availableEntityIds.push(id - 1);
}

EntityId ECS::CreateEntity()
{
    auto newEntity = availableEntityIds.top();
    availableEntityIds.pop();
    return newEntity;
}

void ECS::DestroyEntity(const EntityId entity)
{
    signatures[entity] = 0u;
    for(SystemId sysId = 0; sysId < numberOfSystems; sysId++)
        systems[sysId]->OnEntityDestroyed(entity);        

    compManager.DestroyAllComponents(entity);
    availableEntityIds.push(entity);
}
    
void ECS::UpdateSystems(const float deltaTime)
{
    for(SystemId id = 0; id < numberOfSystems; id++)
        systems[id].get()->Update(deltaTime);
}

void ECS::RenderSystems()
{
    for(SystemId id = 0; id < numberOfSystems; id++)
        systems[id].get()->Render();

}

