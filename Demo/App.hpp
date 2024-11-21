#pragma once
#include "SDL3/SDL_render.h"

class App
{
public:
    App();

    void Update();
    bool ProcessInputs();
    void Render();
    ~App();
private:
    void Initialise();
    void Clean();

	SDL_Texture* texture;
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Event event;
};
