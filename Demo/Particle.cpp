#include "Particle.hpp"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3_image/SDL_image.h"

void Particle::Update()
{

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
