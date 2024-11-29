#pragma once
#include "SDL3/SDL_blendmode.h"
#include "SDL3/SDL_render.h"

struct Position
{
    float x = 0.0, y = 0.0;
};

struct Mass
{
    float mass = 1.0;
};

struct Velocity
{
    float vel_x = 0.0, vel_y = 0.0;   
};

struct Color
{
    SDL_Color color;
};

struct Image
{
    SDL_Texture* texture = nullptr;
    SDL_BlendMode blendMode = SDL_BLENDMODE_NONE;
};

struct AnimationSheet
{
    SDL_Texture* texture;
    int grid_x;
    int grid_y;
};

struct LifeTime
{
    float maxLifeTime = 1.0;
    float remainingLifeTime = maxLifeTime;
};
