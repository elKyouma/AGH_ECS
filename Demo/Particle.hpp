#pragma once
#include "SDL3/SDL_render.h"
#include "Utils.hpp"
#include <string>
#include "Time.hpp"

class Particle
{
public: 
    Particle(float startPosX, float startPosY, SDL_Texture* texture, SDL_Renderer* renderer);
    void Update();
    void Render();

    float TTL = 100.0;
private:
    float posX = .0;
    float posY = .0;
    float velX = .0;
    float velY = .0;
    Time::Timer timer;
    SDL_Texture* texture;
    SDL_Renderer* renderer;
};
