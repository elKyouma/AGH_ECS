#pragma once
#include "ECS.hpp"
#include "SDL3/SDL_render.h"
#include <chrono>
#include "Types.hpp"
#include <chrono>
#include <vector>

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
    void AddNewParticle(size_t i);
    void UpdateParticles();

	SDL_Texture* texture;
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Event event;

    double reloadTime = 1.0 * 5.0 / 300;
    double timer = reloadTime;

    double deltaTime;
    std::chrono::time_point<std::chrono::high_resolution_clock> prevFrameStart{std::chrono::high_resolution_clock::now()};
    ECS ecs{};
    std::vector<EntityId> particles;
};
