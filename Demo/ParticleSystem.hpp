#pragma once
#include "Object.hpp"
#include "Particle.hpp"
#include "Renderable.hpp"
#include "SDL3/SDL_render.h"
#include "Utils.hpp"
#include <vector>
#include <memory>

class ParticleSystem : public Object, public Renderable
{
public:
    ParticleSystem(SDL_Renderer* renderer) : Renderable(renderer) {}
    void Init() override;
    void Update() override;
    void Render() override;
    void Clean() override;
private:
    SDL_Texture* particleTexture;
    std::vector<std::unique_ptr<Object>> particles;
    int numberOfParticles = 1000;
};
