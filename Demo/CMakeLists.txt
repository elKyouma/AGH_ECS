add_executable(App main.cpp App.cpp Utils.cpp Particle.cpp ParticleSystem.cpp Time.cpp)
target_link_libraries(App PRIVATE ECS_Library Dependencies)
target_compile_features(App PRIVATE cxx_std_20)

add_definitions( -DART_PATH=\"${CMAKE_CURRENT_LIST_DIR}/Art/\" )
