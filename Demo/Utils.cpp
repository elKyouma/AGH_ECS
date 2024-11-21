#include "Utils.hpp"
#include "SDL3/SDL_surface.h"
#include "SDL3_image/SDL_image.h"

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
