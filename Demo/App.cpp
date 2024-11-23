#include "App.hpp"
#include "Components.hpp"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "Systems/Gravity.hpp"
#include "Systems/Motion.hpp"
#include "Utils.hpp"
#include <SDL3/SDL.h>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <ctime>
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

void App::Initialise()
{
    srand(time(0));
    std::string path = ART_PATH;
	path += "gandalf.jpg";
    texture = LoadTextureFromFile(renderer, path);
    
    ecs.RegisterComponentPool<Position>();
    ecs.RegisterComponentPool<Velocity>();
    ecs.RegisterComponentPool<Mass>();
    
    particles.resize(200);
    for(size_t i = 0; i < 200; i++)
    {    
        particles[i] = ecs.CreateEntity();
        ecs.AddComponent<Mass>(particles[i]).mass = 5;

        auto& pos = ecs.AddComponent<Position>(particles[i]);
        pos.x = 100;
        pos.y = 100;

        auto& vel = ecs.AddComponent<Velocity>(particles[i]);
        vel.vel_x = (2.0*rand() / RAND_MAX - 1) * 40;
        vel.vel_y = (1.0*rand() / RAND_MAX + 1.0) * 40;
    }
    
    ecs.RegisterSystem<Motion>();
    ecs.RegisterSystem<Gravity>();
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

void App::Update()
{
    {
        auto now = std::chrono::high_resolution_clock::now();
        deltaTime = std::chrono::duration_cast<std::chrono::nanoseconds>(now - prevFrameStart).count() / 1.e9;
        prevFrameStart = now;
    }
    
    ecs.UpdateSystems(deltaTime);
}

void App::Render()
{
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
	SDL_RenderClear(renderer);
    
    for(const auto ent : particles)
    {
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        auto& pos = ecs.GetComponent<Position>(ent);
        SDL_FRect particleRect{
            pos.x - 10,
            (200 - pos.y) - 10,
            20,
            20
        };

	    SDL_RenderTexture(renderer, texture, NULL, &particleRect);
    }

    SDL_RenderPresent(renderer);
}
