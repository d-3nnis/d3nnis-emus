cmake_minimum_required(VERSION 3.20)
project(chip8_emulator VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_SDL_PLATFORM "Build SDL platform" ON)
option(BUILD_TESTS "Build tests" OFF)

include_directories(${CMAKE_SOURCE_DIR}/utils)

add_subdirectory(emus)

if(BUILD_SDL_PLATFORM)
  add_subdirectory(vendor)
  add_subdirectory(platform)
endif()
