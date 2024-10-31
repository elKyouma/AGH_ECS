cmake_minimum_required(VERSION 3.15)

project(
  ECS_DEMO
  VERSION 1.0
  LANGUAGES CXX)

# Including header files here helps IDEs but is not required.
# Output libname matches target name, with the usual extensions on your system
add_library(Library ECS/World.cpp ECS/System.hpp ECS/Component.hpp ECS/Entity.hpp)


# Create an option to switch between a system sdl library and a vendored SDL library
option(MYGAME_VENDORED "Use vendored libraries" OFF)

if(MYGAME_VENDORED)
    # This assumes you have added SDL as a submodule in vendored/SDL
    add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
else()
    # 1. Look for a SDL3 package,
    # 2. look for the SDL3-shared component, and
    # 3. fail if the shared component cannot be found.
    find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
endif()
# Adding something we can run - Output name matches target name
add_executable(App Demo/main.cpp)

target_link_libraries(App PRIVATE Library SDL3::SDL3)
