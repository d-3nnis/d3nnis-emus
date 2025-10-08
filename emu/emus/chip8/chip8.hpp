#include <cstdint>

static constexpr int MEMORY_SIZE = 4096;
static constexpr uint8_t MEMORY[MEMORY_SIZE] = {};

static constexpr int CHIP8_DISPLAY_WIDTH = 64;
static constexpr int CHIP8_DISPLAY_HEIGHT = 32;

void startChip8Emu(void);
