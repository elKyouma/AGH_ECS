#include "App.hpp"
#include "Components.hpp"
#include "SDL3/SDL_blendmode.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"
#include "Systems/Gravity.hpp"
#include "Systems/Motion.hpp"
#include "Systems/SpriteColorizer.hpp"
#include "Types.hpp"
#include "Utils.hpp"
#include <SDL3/SDL.h>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <execution>
#include <filesystem>
#include <string>


App::App()
{
	if (!SDL_Init(SDL_INIT_VIDEO))
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
	if (!SDL_CreateWindowAndRenderer("Hello SDL", 320, 240, SDL_WINDOW_RESIZABLE, &window, &renderer))
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window and renderer: %s", SDL_GetError()); 

    Initialise();
}

App::~App()
{
    Clean();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

    
void App::AddNewParticle(size_t i)
{
    particles[i] = ecs.CreateEntity();
    ecs.AddComponent<Mass>(particles[i], 0);
    ecs.AddComponent<LifeTime>(particles[i], 1, 1);
    ecs.AddComponent<Image>(particles[i], texture, SDL_BLENDMODE_ADD);
    ecs.AddComponent<Color>(particles[i], SDL_Color{255,100,60,120});
    ecs.AddComponent<Position>(particles[i], 130 + 20*(2.0*rand() / RAND_MAX - 1), 20);
    

    auto& vel = ecs.AddComponent<Velocity>(particles[i]);
    float speed = 75 + 40 * (2.0*rand() / RAND_MAX -1);

    vel.vel_x = (2.0*rand() / RAND_MAX - 1) / 2;
    vel.vel_y = 1;
    const auto max = sqrt(vel.vel_x * vel.vel_x + vel.vel_y * vel.vel_y);
    vel.vel_x *= speed / max;
    vel.vel_y *= speed / max;
}

void App::Initialise()
{
    srand(time(0));
    std::string path = ART_PATH;
	path += "gandalf.jpg";
    texture = LoadTextureFromFile(renderer, path);
    
    ecs.RegisterComponentPool<Position>();
    ecs.RegisterComponentPool<Velocity>();
    ecs.RegisterComponentPool<Color>();
    ecs.RegisterComponentPool<LifeTime>();
    ecs.RegisterComponentPool<Image>();
    ecs.RegisterComponentPool<Mass>();
    
    particles.reserve(300);

    ecs.RegisterSystem<Motion>();
    ecs.RegisterSystem<Gravity>();
    ecs.RegisterSystem<SpriteColorizer>(renderer);
}

void App::Clean()
{ 
	SDL_DestroyTexture(texture);
}

bool App::ProcessInputs()
{
    bool isQuiting = false;
    SDL_PollEvent(&event);
	
    if (event.type == SDL_EVENT_QUIT)
		isQuiting = true;

    return isQuiting;
}

void App::UpdateParticles()
{
    for(size_t i = 0; i < particles.size(); i++)
    {
        auto& lifetime = ecs.GetComponent<LifeTime>(particles[i]);
        lifetime.remainingLifeTime -= deltaTime;
        if(lifetime.remainingLifeTime < 0)
        {    
            ecs.DestroyEntity(particles[i]);
            AddNewParticle(i);
        }
    
        float progress = lifetime.remainingLifeTime / lifetime.maxLifeTime;
        SDL_Color color = SDL_Color{Uint8(255 * progress),
                    Uint8(100 * progress/2 + 50),
                    Uint8(60 * progress/2 + 50),
                    Uint8(170 * progress)};
        ecs.GetComponent<Color>(particles[i]).color = color;
    }
}

void App::Update()
{
    {
        auto now = std::chrono::high_resolution_clock::now();
        deltaTime = std::chrono::duration_cast<std::chrono::nanoseconds>(now - prevFrameStart).count() / 1.e9;
        prevFrameStart = now;
    }
    
    timer -= deltaTime;

    if(timer < 0)
    {
        timer = reloadTime;
        if(particles.size() != particles.capacity())
        {   
            for(int i = 0; i < 5; i++)
            {
                particles.push_back(0);
                AddNewParticle(particles.size()-1);
            }
        }
    }

    UpdateParticles();

    ecs.UpdateSystems(deltaTime);
}

void App::Render()
{
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
	SDL_RenderClear(renderer);
    ecs.RenderSystems();    
    SDL_RenderPresent(renderer);
}
