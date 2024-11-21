#pragma once

#include "SDL3/SDL_render.h"
#include <string>

SDL_Texture* LoadTextureFromFile(SDL_Renderer* renderer, std::string path);
