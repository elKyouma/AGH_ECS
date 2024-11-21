#include "App.hpp"
#include <SDL3/SDL.h>
#include "Benchmark.hpp"
#include <fstream>
#include <string>

int main(int argc, char* argv[])
{
    App app;
    //std::ofstream benchmarkFile("benchmark.txt");
    //int it = 0;
	bool isQuiting = false;
    while (!isQuiting) {
		//Benchmark benchmark;
        //benchmark.Start();
        isQuiting = app.Update();
        app.Render();

        //auto benchmarkData =benchmark.Measure();
        //benchmarkFile << it << '\t' << benchmarkData.miliSec << '\t' << benchmarkData.megaCycles << '\n'; 
        //it++;
	}
    //benchmarkFile.close();

	return 0;
}
