#include "Time.hpp"

namespace Time
{
	double deltaTime = 0.0;

	Timer::Timer()
	{
		lastTime = SDL_GetTicks();
	}

	double Timer::Reset()
	{
		uint64_t newTime = SDL_GetTicks();
		deltaTime = (newTime - lastTime) / 1000.0;
		lastTime = newTime;
		return deltaTime;
	}

	double Timer::Measure()
	{
		uint64_t newTime = SDL_GetTicks();
		deltaTime = (newTime - lastTime) / 1000.0;
		return deltaTime;
	}
};
