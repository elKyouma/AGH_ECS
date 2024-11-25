#pragma once

#include "System.hpp"
#include "Components.hpp"
#include "Types.hpp"

class SpriteColorizer : public System
{
    virtual void SetSignature(Signature& systemSign) override
    {
        systemSign.set(compManager->CompId<Position>());
        systemSign.set(compManager->CompId<Image>());
        systemSign.set(compManager->CompId<Color>());
    }

};
