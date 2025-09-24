set(DISPLAY_SOURCES
    SDLDisplay.cpp
)

add_library(display_lib ${DISPLAY_SOURCES})
target_include_directories(display_lib PUBLIC .)

target_link_libraries(display_lib PRIVATE SDL3::SDL3)
