# define project
cmake_minimum_required(VERSION 3.31)
project(clock-overlay)
add_executable(clock-overlay src/main.c)

# link SDL dependencies
set(SDLTTF_VENDORED ON)
add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
add_subdirectory(vendored/SDL_ttf EXCLUDE_FROM_ALL)
target_link_libraries(clock-overlay PRIVATE SDL3::SDL3 SDL3_ttf::SDL3_ttf)
add_custom_command(
  TARGET clock-overlay POST_BUILD
  COMMAND "${CMAKE_COMMAND}" -E copy
    $<TARGET_FILE:SDL3::SDL3-shared>
    $<TARGET_FILE:SDL3_ttf::SDL3_ttf-shared>
    $<TARGET_FILE_DIR:clock-overlay>
  VERBATIM
)
