#include "ParticleSystem.hpp"
#include "Utils.hpp"
#include <vector>

void ParticleSystem::Init()
{
    std::string path = ART_PATH;
	path += "gandalf.jpg";
    particleTexture = LoadTextureFromFile(renderer, path);

    for(int i=0; i < numberOfParticles; i++)
    {
        particles.emplace_back(
            1.0 * Rand(-100.0, 100.0), 
            1.0 * Rand(-100.0, 100.0), 
            particleTexture, renderer);
    }
}

void ParticleSystem::Update()
{
    for(auto& particle : particles)
        particle.Update();
    for (int i = 0; i < particles.size(); i++) 
    {
        if (particles[i].TTL <= 0)
        {
            particles.erase(particles.begin() + i);
            particles.emplace_back(
            1.0 * Rand(-100.0, 100.0), 
            1.0 * Rand(-100.0, 100.0), 
            particleTexture, renderer);
        }
    }
}

void ParticleSystem::Render()
{
    for(auto& particle : particles)
        particle.Render();
}

void ParticleSystem::Clean()
{
	SDL_DestroyTexture(particleTexture);
}
