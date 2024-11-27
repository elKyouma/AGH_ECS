#pragma once

#include <SDL3/SDL_render.h>
#include <iostream>
#include "System.hpp"
#include "Components.hpp"
#include "Types.hpp"

class SpriteColorizer : public System
{
public:
    SpriteColorizer(SDL_Renderer* renderer) : renderer(renderer)
    {}

private:
    virtual void SetSignature(Signature& systemSign) override
    {
        systemSign.set(compManager->CompId<Position>());
        systemSign.set(compManager->CompId<Image>());
    }

    virtual void Render() override
    {
        const auto& pos = compManager->GetComponentPool<Position>();
        const auto& images = compManager->GetComponentPool<Image>();
        for(auto ent : entities )
        {
            SDL_FRect particleRect{
                pos.GetComponent(ent).x - 10,
                (200 - pos.GetComponent(ent).y) - 10,
                20,
                20};
            SDL_Texture* texture = images.GetComponent(ent).texture;
            auto blendMode = images.GetComponent(ent).blendMode;
            SDL_SetTextureBlendMode(texture, blendMode);
            auto colorOpt = compManager->TryGetComponent<Color>(ent);
            if(colorOpt.has_value())
            {
                const SDL_Color color = colorOpt->get().color;
                SDL_SetTextureColorMod(texture, color.r, color.g, color.b); 
                SDL_SetTextureAlphaMod(texture, color.a); 
            }
	        SDL_RenderTexture(renderer, texture, NULL, &particleRect);   
        }
    }

private:
    SDL_Renderer* renderer;
};
