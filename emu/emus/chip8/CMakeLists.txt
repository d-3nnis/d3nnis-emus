set(CHIP8_SOURCES
    chip8.cpp
)

set(CHIP8_HEADERS
    chip8.hpp
)

add_library(chip8_lib ${CHIP8_SOURCES})
target_include_directories(chip8_lib PUBLIC .)

target_link_libraries(chip8_lib PRIVATE display_lib)
