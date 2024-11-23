#include "ParticleSystem.hpp"

void ParticleSystem::Init()
{
    std::string path = ART_PATH;
	path += "gandalf.jpg";
    particleTexture = LoadTextureFromFile(renderer, path);

    for(int i=0; i < numberOfParticles; i++)
    {
        particles.emplace_back(0.0 + 10*i, 0.0, particleTexture, renderer);
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
