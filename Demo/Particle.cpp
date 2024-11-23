#include "Particle.hpp"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3_image/SDL_image.h"
#include "Utils.hpp"

Particle::Particle(float startPosX, float startPosY, SDL_Texture* texture, SDL_Renderer* renderer)
    : posX(startPosX), posY(startPosY), texture(texture), renderer(renderer) 
{
    timer.Reset();
    velX = Rand(-5, 5);
    velY = Rand(-5, 5);
}

void Particle::Update()
{       
    double dt = timer.Measure();
    posX += velX * dt;
    posY -= velY * dt - (3.0 * dt * dt / 2.0);
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
