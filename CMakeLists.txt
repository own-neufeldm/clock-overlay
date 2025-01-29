# project outline
cmake_minimum_required(VERSION 3.29)
project(clock-overlay)

# dependencies
find_package(SDL3 CONFIG REQUIRED)
set(SDLTTF_VENDORED ON)
add_subdirectory(vendored/SDL_ttf EXCLUDE_FROM_ALL)

# target
add_executable("${CMAKE_PROJECT_NAME}" src/main.c)
target_link_libraries(
  "${CMAKE_PROJECT_NAME}" PRIVATE
  SDL3::SDL3
  SDL3_ttf::SDL3_ttf
)
add_custom_command(
  TARGET "${CMAKE_PROJECT_NAME}" POST_BUILD
  COMMAND "${CMAKE_COMMAND}" -E copy
    "$<TARGET_FILE:SDL3_ttf::SDL3_ttf-shared>"
    "$<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>"
  VERBATIM
)
