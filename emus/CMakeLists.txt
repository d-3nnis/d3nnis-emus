set(CHIP8_SOURCES src/chip8.cpp)

add_library(chip8_lib STATIC ${CHIP8_SOURCES})

target_include_directories(
  chip8_lib
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc/chip8>
         $<INSTALL_INTERFACE:inc/chip8>
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
