#pragma once
#include "Particle.hpp"
#include "SDL3/SDL_render.h"
#include "Utils.hpp"
#include <vector>

class ParticleSystem
{
public:
    ParticleSystem(SDL_Renderer* renderer) : renderer(renderer) {}
    void Init();
    void Update();
    void Render();
    void Clean();
private:
    SDL_Renderer* renderer;
    SDL_Texture* particleTexture;
    std::vector<Particle> particles;
    int numberOfParticles = 10;
};
