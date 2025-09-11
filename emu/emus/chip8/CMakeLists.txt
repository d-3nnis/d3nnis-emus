set(CHIP8_SOURCES
    chip8.cpp
)

set(CHIP8_HEADERS
    chip8.hpp
)

# Create library (no main.cpp here)
add_library(chip8_lib ${CHIP8_SOURCES})
target_include_directories(chip8_lib PRIVATE ${CHIP8_HEADERS})

target_link_libraries(chip8_lib PRIVATE SDL3::SDL3)
