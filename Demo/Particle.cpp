#include "Particle.hpp"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3_image/SDL_image.h"
#include "Utils.hpp"

Particle::Particle(float startPosX, float startPosY, SDL_Texture* texture, SDL_Renderer* renderer)
    : posX(startPosX), posY(startPosY), texture(texture), renderer(renderer) 
{
    velX = Rand(-10, 10);
    velY = Rand(-10, 10);
}

void Particle::Update()
{
    velY -= 0.1;
    posX += velX;
    posY += velY;
}

void Particle::Render()
{
    SDL_FRect rect; 
    rect.x = posX + WIDTH / 2.0 - 50; 
    rect.y = posY + HEIGHT / 2.0 - 50; 
    rect.w = 100; 
    rect.h = 100;

    SDL_RenderTexture(renderer, texture, NULL, &rect);
}
