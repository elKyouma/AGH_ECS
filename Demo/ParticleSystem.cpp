#include "ParticleSystem.hpp"
#include "Utils.hpp"

void ParticleSystem::Init()
{
    std::string path = ART_PATH;
	path += "gandalf.jpg";
    particleTexture = LoadTextureFromFile(renderer, path);

    for(int i=0; i < numberOfParticles; i++)
    {
        particles.emplace_back(1.0 * Rand(-300, 300), 1.0 * Rand(-300, 300), particleTexture, renderer);
    }
}

void ParticleSystem::Update()
{
    for(auto& particle : particles)
        particle.Render();
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
