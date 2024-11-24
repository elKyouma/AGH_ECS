#include "Utils.hpp"
#include "SDL3/SDL_surface.h"
#include "SDL3_image/SDL_image.h"
#include <cstdlib>

SDL_Texture* LoadTextureFromFile(SDL_Renderer* renderer, std::string path)
{
    SDL_Texture* texture;
    SDL_Surface* surface;
	surface = IMG_Load(path.c_str());
    if (!surface) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create surface from image: %s", SDL_GetError());
		return nullptr;
	}

	texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture from surface: %s", SDL_GetError());
		return nullptr;
	}

	SDL_DestroySurface(surface);
    return texture;
}

int Rand(int minv, int maxv)
{
    if(minv < 0)
        return std::rand() % (2 * maxv) + minv;
    return std::rand() % maxv + minv;
}

double Rand(double minv, double maxv)
{
    if(minv < 0.0)
        return (std::rand() % static_cast<int>(2 * maxv * 100) + minv * 100) / 100.0;
    
    return (std::rand() % static_cast<int>(maxv * 100) + minv * 100) / 100.0;
}
