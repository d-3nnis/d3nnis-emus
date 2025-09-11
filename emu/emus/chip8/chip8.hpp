// #include "SDL3/SDL.h"
#include <SDL3/SDL.h>
#include <cstdint>
#include <memory>

static constexpr int MEMORY_SIZE = 4096;
static constexpr uint8_t MEMORY[MEMORY_SIZE] = {};

static constexpr int DISPLAY_WIDTH = 64;
static constexpr int DISPLAY_HEIGHT = 32;


void startChip8Emu(void);
