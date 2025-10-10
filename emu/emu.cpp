#include <cstdio>
#include <filesystem>
#include <fstream>
#include <vector>
#include "emus/chip8/chip8.hpp"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <program_path>\n", argv[0]);
        return 1;
    }
    
    std::filesystem::path programPath = argv[1];
    if(!std::filesystem::exists(programPath) || !std::filesystem::is_regular_file(programPath)) {
        printf("File does not exist or is not a regular file: %s\n", programPath.c_str());
    }
    std::ifstream rom(programPath, std::ios::in | std::ios::binary);
    if(rom.fail()) {
        printf("Failed to open file: %s\n", programPath.c_str());
        return 1;
    }

    std::vector<uint8_t> romBuffer((std::istreambuf_iterator<char>(rom)), std::istreambuf_iterator<char>());
    rom.close();

    Chip8 chip8;
    chip8.loadProgram(romBuffer);
    chip8.startChip8Emu();
    return 0;
}
