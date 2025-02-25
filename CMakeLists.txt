# define project
cmake_minimum_required(VERSION 3.29)
project(clock-overlay)

# ensure dependencies
find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_image CONFIG REQUIRED)
find_package(SDL3_ttf CONFIG REQUIRED)

# create executable
add_executable("${CMAKE_PROJECT_NAME}" src/main.c src/app.c)
target_link_libraries(
  "${CMAKE_PROJECT_NAME}" PRIVATE
  SDL3::SDL3-shared
  SDL3_image::SDL3_image-shared
  SDL3_ttf::SDL3_ttf
)
