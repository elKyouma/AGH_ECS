#pragma once
#include <SDL3/SDL_render.h>

class Renderable
{
public:
    virtual void Render() =0;
protected:
    Renderable(SDL_Renderer* renderer) : renderer(renderer) {}
    SDL_Renderer* renderer;
};
