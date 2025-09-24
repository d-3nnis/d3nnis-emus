cmake_minimum_required(VERSION 3.10)
project(chip8_emulator VERSION 1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# add_subdirectory(SDL EXCLUDE_FROM_ALL)
add_subdirectory(vendor)
add_subdirectory(display)
add_subdirectory(emus)

add_executable(chip8_emulator emu.cpp)
# target_link_libraries(chip8_emulator PUBLIC SDL3::SDL3)
target_link_libraries(chip8_emulator PRIVATE display_lib)
target_link_libraries(chip8_emulator PRIVATE chip8_lib)

# find_package(SDL2 REQUIRED)
