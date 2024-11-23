#pragma once
#include "System.hpp"
#include "Components.hpp"
#include "Types.hpp"

class Gravity : public System
{
    virtual void SetSignature(Signature& systemSign) override
    {
        systemSign.set(compManager->CompId<Velocity>());
        systemSign.set(compManager->CompId<Mass>());
    }

    virtual void Update(const float deltaTime) override
    {
        auto& velocity = compManager->GetComponentPool<Velocity>();
        auto& mass = compManager->GetComponentPool<Mass>();
        
        for(const auto ent : entities)
            velocity.GetComponent(ent).vel_y += gravityForce * mass.GetComponent(ent).mass * deltaTime;
    }

private:
    float gravityForce = -9.81f;
};
