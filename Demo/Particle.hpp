#pragma once
#include "SDL3/SDL_render.h"
#include "Utils.hpp"
#include <string>

class Particle
{
public: 
    Particle(float startPosX, float startPosY, SDL_Texture* texture, SDL_Renderer* renderer);
    void Update();
    void Render();
private:
    float posX = .0;
    float posY = .0;
    float velX = .0;
    float velY = .0;
    SDL_Texture* texture;
    SDL_Renderer* renderer;
};
