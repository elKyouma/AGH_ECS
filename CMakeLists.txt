cmake_minimum_required(VERSION 3.15)

project(
  ECS_DEMO
  VERSION 1.0
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(ECS EXCLUDE_FROM_ALL)
add_subdirectory(Dependency EXCLUDE_FROM_ALL)
add_subdirectory(Demo)
enable_testing()
add_subdirectory(Tests)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(PREDEFINED_TARGETS_FOLDER "CustomTargets")
