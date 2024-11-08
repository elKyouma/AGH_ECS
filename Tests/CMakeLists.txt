set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_executable(
  unit_tests
  test.cpp
)

include_directories(
  ${CMAKE_SOURCE_DIR}/Dependency/googletest/include 
  ${CMAKE_SOURCE_DIR}/Dependency/googletest
)

target_link_libraries(
  unit_tests
  PRIVATE
  ECS_Library 
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(unit_tests)
