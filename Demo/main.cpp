#include "Benchmark.hpp"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <fstream>
#include <string>

int main(int argc, char* argv[])
{
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Surface* surface;
	SDL_Texture* texture;
	SDL_Event event;

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
		return 3;
	}

	if (!SDL_CreateWindowAndRenderer("Hello SDL", 320, 240, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window and renderer: %s", SDL_GetError());
		return 3;
	}

	std::string path = ART_PATH;
	path += "gandalf.jpg";
	surface = IMG_Load(path.c_str());

	if (!surface) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create surface from image: %s", SDL_GetError());
		return 3;
	}
	texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (!texture) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture from surface: %s", SDL_GetError());
		return 3;
	}
	SDL_DestroySurface(surface);

    std::ofstream benchmarkFile("benchmark.txt");
    int it = 0;
	while (1) {
		Benchmark benchmark;
        benchmark.Start();
        SDL_PollEvent(&event);
		if (event.type == SDL_EVENT_QUIT) {
			break;
		}
		SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
		SDL_RenderClear(renderer);
		SDL_RenderTexture(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);

        auto benchmarkData =benchmark.Measure();
        benchmarkFile << it << '\t' << benchmarkData.miliSec << '\t' << benchmarkData.megaCycles << '\n'; 
        it++;
	}
    benchmarkFile.close();
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();

	return 0;
}
