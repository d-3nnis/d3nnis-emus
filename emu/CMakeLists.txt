cmake_minimum_required(VERSION 3.10)
project(chip8_emulator VERSION 1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(vendor)

include_directories(${CMAKE_SOURCE_DIR}/utils)
add_subdirectory(display)
add_subdirectory(emus)

add_executable(chip8_emulator emu.cpp)
target_link_libraries(chip8_emulator PRIVATE display_lib chip8_lib)

