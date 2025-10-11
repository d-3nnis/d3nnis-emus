set(PLATFORM_SOURCES src/Chip8SDLPlatform.cpp src/Chip8SDLAudio.cpp)

add_library(chip8_sdl_platform STATIC ${PLATFORM_SOURCES})

target_include_directories(
  chip8_sdl_platform
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(chip8_sdl_platform PUBLIC SDL3::SDL3 chip8_lib)

add_executable(chip8_emulator main.cpp)
target_link_libraries(chip8_emulator PRIVATE chip8_sdl_platform)
