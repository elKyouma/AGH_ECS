#include "App.hpp"
#include "ParticleSystem.hpp"
#include "SDL3_image/SDL_image.h"
#include "Utils.hpp"
#include <SDL3/SDL.h>
#include <chrono>
#include <memory>
#include <string>


App::App()
{
	if (!SDL_Init(SDL_INIT_VIDEO))
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
	if (!SDL_CreateWindowAndRenderer("Hello SDL", WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer))
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
    particleSystem = std::make_unique<ParticleSystem>(renderer);
    particleSystem->Init();
}

void App::Clean()
{ 
    particleSystem->Clean();
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

    particleSystem->Update();
}

void App::Render()
{
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
	SDL_RenderClear(renderer);
    particleSystem->Render();
	SDL_RenderPresent(renderer);
}
