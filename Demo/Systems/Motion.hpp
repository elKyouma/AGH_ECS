#pragma once
#include "System.hpp"
#include "Components.hpp"
#include "Types.hpp"

class Motion : public System
{
    virtual void SetSignature(Signature& systemSign) override
    {
        systemSign.set(compManager->CompId<Position>());
        systemSign.set(compManager->CompId<Velocity>());
    }

    virtual void Update() override
    {
        auto& positions = compManager->GetComponentPool<Position>();
        auto& velocity = compManager->GetComponentPool<Velocity>();
        for(const auto ent : entities)
        {
            positions.GetComponent(ent).x += velocity.GetComponent(ent).vel_x;
            positions.GetComponent(ent).y += velocity.GetComponent(ent).vel_y;
        }
    }
};
