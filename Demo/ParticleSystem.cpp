#include "ParticleSystem.hpp"
#include "Particle.hpp"
#include "Utils.hpp"
#include <cmath>
#include <memory>
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

        particles.push_back(std::make_unique<Particle>(
            r * std::cos(fi), 
            400.0 + r * std::sin(fi), 
            particleTexture, renderer));
    }
}

void ParticleSystem::Update()
{
    for(auto& particle : particles)
        particle->Update();
    for (int i = 0; i < particles.size(); i++) 
    {
        if (dynamic_cast<Particle*>(particles[i].get())->TTL <= 0)
        {
            float r = Rand(0.0, 50.0);
            float fi = Rand(0.0, 359.0);
            
            particles.erase(particles.begin() + i);
            particles.push_back(std::make_unique<Particle>(
                r * std::cos(fi), 
                400.0 + r * std::sin(fi), 
                particleTexture, renderer));
        }
    }
}

void ParticleSystem::Render()
{
    for(auto& particle : particles)
        dynamic_cast<Particle*>(particle.get())->Render();
}

void ParticleSystem::Clean()
{
	SDL_DestroyTexture(particleTexture);
}
