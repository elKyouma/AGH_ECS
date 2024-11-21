#include "App.hpp"
#include "SDL3_image/SDL_image.h"
#include "Utils.hpp"
#include <SDL3/SDL.h>
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
    std::string path = ART_PATH;
	path += "gandalf.jpg";
    texture = LoadTextureFromFile(renderer, path);
}

void App::Clean()
{ 
	SDL_DestroyTexture(texture);
}

bool App::Update()
{
    bool isQuiting = false;
    SDL_PollEvent(&event);
	
    if (event.type == SDL_EVENT_QUIT)
		isQuiting = true;

    return isQuiting;
}

void App::Render()
{
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
	SDL_RenderClear(renderer);
	SDL_RenderTexture(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}
