#include "SDL3/SDL_render.h"

struct Position
{
    double x, y;
};

struct Mass
{
    double mass;
};

struct Velocity
{
    double vel_x, vel_y;   
};

struct Color
{
    SDL_Color color;
};

struct Image
{
    SDL_Texture* texture;
};

struct AnimationSheet
{
    SDL_Texture* texture;
    int grid_x;
    int grid_y;
};
