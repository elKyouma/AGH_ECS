#pragma once
#include "Object.hpp"
#include "Particle.hpp"
#include "Renderable.hpp"
#include "SDL3/SDL_render.h"
#include "Utils.hpp"
#include <vector>

class ParticleSystem : public Object, public Renderable
{
public:
    ParticleSystem(SDL_Renderer* renderer) : Renderable(renderer) {}
    void Init();
    void Update();
    void Render() ;
    void Clean();
private:
    SDL_Texture* particleTexture;
    std::vector<Particle> particles;
    int numberOfParticles = 1000;
};
