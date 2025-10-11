set(PLATFORM_SOURCES Chip8SDLPlatform.cpp Chip8SDLAudio.cpp)
set(PLATFORM_HEADERS Chip8SDLPlatform.hpp Chip8SDLAudio.hpp)

add_library(chip8_sdl_platform ${PLATFORM_SOURCES} ${PLATFORM_HEADERS})
target_include_directories(chip8_sdl_platform PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(chip8_sdl_platform PUBLIC SDL3::SDL3 chip8_lib)
