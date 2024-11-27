#pragma once
#include "Object.hpp"
#include "Renderable.hpp"
#include "SDL3/SDL_render.h"
#include "Utils.hpp"
#include <string>
#include "Time.hpp"

class Particle : public Object, public Renderable
{
public: 
    Particle(float startPosX, float startPosY, SDL_Texture* texture, SDL_Renderer* renderer);

    void Init() override {}
    void Update() override;
    void Clean() override {}
    void Render() override;

    float TTL = 100.0;
private:
    float posX = .0;
    float posY = .0;
    float velX = .0;
    float velY = .0;
    Time::Timer timer;
    SDL_Texture* texture;
};
