#pragma once

#include "SDL3/SDL_render.h"
#include <string>

#define WIDTH 1280
#define HEIGHT 1024

SDL_Texture* LoadTextureFromFile(SDL_Renderer* renderer, std::string path);

int Rand(int minv, int maxv);
