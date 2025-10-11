#include "Chip8SDLPlatform.hpp"
#include "emus/chip8/chip8.hpp"
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <program_path> <instructions per update>\n", argv[0]);
        return 1;
    }

    std::filesystem::path romPath = argv[1];
    int instructionsPerUpdate = std::stoi(argv[2]);
    if (!std::filesystem::exists(romPath) ||
        !std::filesystem::is_regular_file(romPath)) {
        std::cerr << "File does not exist or is not a regular file:" << romPath
                  << "\n";
        return 1;
    }
    std::ifstream rom(romPath, std::ios::in | std::ios::binary);
    if (rom.fail()) {
        std::cerr << "Failed to open file:" << romPath.c_str() << "\n";
        return 1;
    }

    std::vector<uint8_t> romBuffer((std::istreambuf_iterator<char>(rom)),
                                   std::istreambuf_iterator<char>());
    rom.close();

    Chip8 emulator;
    emulator.loadProgram(romBuffer);

    Chip8SDLPlatform::Config chip8Config = {
        .displayPixelWidth = 64,
        .displayPixelHeight = 32,
        .displayScale = 10,
        .instructionsPerFrame = instructionsPerUpdate,
    };
    Chip8SDLPlatform platform(chip8Config);
    platform.run(emulator);
    return 0;
}
