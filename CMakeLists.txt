cmake_minimum_required(VERSION 3.5)
project(imgui)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

# Create an option to switch between a system sdl library and a vendored SDL library
option(MYGAME_VENDORED "Use vendored libraries" OFF)
include_directories(C:/devel/SDL/include)
if(MYGAME_VENDORED)
    # This assumes you have added SDL as a submodule in vendored/SDL
    add_subdirectory( EXCLUDE_FROM_ALL)
else()
    # 1. Look for a SDL3 package,
    # 2. look for the SDL3-shared component, and
    # 3. fail if the shared component cannot be found.
    find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
endif()

# Create your game executable target as usual
add_executable(${PROJECT_NAME} WIN32 src/main.c)

# Link to the actual SDL3 library.
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)
