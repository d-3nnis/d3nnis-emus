set(CHIP8_SOURCES chip8.cpp)

set(CHIP8_HEADERS chip8.hpp)

add_library(chip8_lib ${CHIP8_SOURCES} ${CHIP8_HEADERS})
target_include_directories(chip8_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# target_link_libraries(chip8_lib PRIVATE chip8_sdl_platform)
