#include "Particle.hpp"
#include "Renderable.hpp"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3_image/SDL_image.h"
#include "Utils.hpp"
#include <cstdint>

Particle::Particle(float startPosX, float startPosY, SDL_Texture* texture, SDL_Renderer* renderer)
    : Renderable(renderer), posX(startPosX), posY(startPosY), texture(texture)  
{
    timer.Reset();
    velX = Rand(-5.0, 5.0);
    velY = Rand(0.0, 5.0);
    TTL = Rand(50.0, 150.0);
}

void Particle::Update()
{       
    double dt = timer.Measure();
    float aX = 0.05;
    if(velX > 0.0) aX = -0.05;
    else if(velX == 0.0) aX = 0.0;

    velX += aX * dt;

    posX += velX * dt;
    posY -= velY * dt + (2.0 * dt * dt / 2.0);

    TTL -= dt;
}

void Particle::Render()
{
    SDL_FRect rect; 
    rect.x = posX + WIDTH / 2.0 - 50; 
    rect.y = posY + HEIGHT / 2.0 - 50; 
    rect.w = 10; 
    rect.h = 10;
    
    int g;
    g = TTL + 105;
    /*if(g < 0) g = 0;*/
    SDL_SetTextureColorMod(texture, 255, g, 0);
    SDL_RenderTexture(renderer, texture, NULL, &rect);
}
