#include "ParticleSystem.hpp"
#include "Utils.hpp"
#include <cmath>
#include <vector>

void ParticleSystem::Init()
{
    std::string path = ART_PATH;
	path += "gandalf.jpg";
    particleTexture = LoadTextureFromFile(renderer, path);
    
    for(int i=0; i < numberOfParticles; i++)
    {
        float r = Rand(0.0, 50.0);
        float fi = Rand(0.0, 359.0);

        particles.emplace_back(
            r * std::cos(fi), 
            400.0 + r * std::sin(fi), 
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
            float r = Rand(0.0, 50.0);
            float fi = Rand(0.0, 359.0);
            
            particles.erase(particles.begin() + i);
            particles.emplace_back(
            r * std::cos(fi), 
            400.0 + r * std::sin(fi), 
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
