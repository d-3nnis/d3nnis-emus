#pragma once

#include "common.hpp"
#include <array>
#include <byteswap.h>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <limits>
#include <random>
#include <span>
#include <stdexcept>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

class Chip8 {
  private:
    struct Chip8Hardware {
        static constexpr int STACK_SIZE = 64;
        static constexpr int MEMORY_SIZE = 4096;
        static constexpr int REGISTER_COUNT = 16;
        static constexpr int KEY_COUNT = 16;
        static constexpr int FONT_SET_START = 0x0;
        static constexpr int FONT_SET_SIZE = 0x80;
        static constexpr int PROGRAM_START = 0x200;
        static constexpr int DISPLAY_START = 0xF00;
        // in bytes
        static constexpr int DISPLAY_SIZE = 0x100;

        std::array<uint8_t, MEMORY_SIZE> MEMORY = {};
        uint8_t REGISTERS[REGISTER_COUNT] = {};
        uint8_t STACK[STACK_SIZE] = {};
        uint16_t KEY_STATE = 0;
        uint16_t PC = PROGRAM_START;
        uint16_t SP = 0;
        // only 12 bits
        uint16_t I = 0;
        uint8_t DELAY_TIMER = 0;
        uint8_t SOUND_TIMER = 0;

        void reset() {
            KEY_STATE = 0;
            PC = PROGRAM_START;
            SP = 0;
            I = 0;
            DELAY_TIMER = 0;
            SOUND_TIMER = 0;
        }
    };

    // probably not the best way to do this
    struct Chip8Sprites {
        static constexpr int SPRITE_MEMORY_SIZE = 80;
        static constexpr int SPRITE_HEIGHT = 5;
        static constexpr std::array<uint8_t, SPRITE_MEMORY_SIZE> sprites = {
            0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
            0x20, 0x60, 0x20, 0x20, 0x70, // 1
            0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
            0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
            0x90, 0x90, 0xF0, 0x10, 0x10, // 4
            0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
            0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
            0xF0, 0x10, 0x20, 0x40, 0x40, // 7
            0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
            0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
            0xF0, 0x90, 0xF0, 0x90, 0x90, // A
            0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
            0xF0, 0x80, 0x80, 0x80, 0xF0, // C
            0xE0, 0x90, 0x90, 0x90, 0xE0, // D
            0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
            0xF0, 0x80, 0xF0, 0x80, 0x80  // F
        };
    };

    const uint8_t *spriteAddress(int spriteIndex) {
        if (spriteIndex < 0 || spriteIndex > 0xF) {
            throw std::runtime_error("Sprite index out of bounds\n");
        }
        return &Chip8Sprites::sprites[spriteIndex *
                                      Chip8Sprites::SPRITE_HEIGHT];
    }

    static constexpr int CHIP8_DISPLAY_WIDTH = 64;
    static constexpr int CHIP8_DISPLAY_HEIGHT = 32;

  public:
    // DO NOT CHANGE, TIMERS RELY ON THIS
    static constexpr int TARGET_FPS = 60;
    static constexpr int FRAME_TIME_MS = 1000 / TARGET_FPS;

    Chip8() : hardware() {
        assert(CHIP8_DISPLAY_HEIGHT * CHIP8_DISPLAY_WIDTH ==
               Chip8Hardware::DISPLAY_SIZE * BITS_PER_BYTE);
        // TODO improve me
        memcpy(hardware.MEMORY.data(), Chip8Sprites::sprites.data(),
               Chip8Sprites::SPRITE_MEMORY_SIZE);

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<uint8_t> dist(
            std::numeric_limits<uint8_t>::min(),
            std::numeric_limits<uint8_t>::max());
    }

    enum class Status {
        OK,
        ROM_OVERSIZED,
        UNRECOGNIZED_KEY,
        INVALID_INSTRUCTION,
        ERROR,
    };

    Status loadProgram(std::vector<uint8_t> program);

    const std::span<const uint8_t, Chip8Hardware::DISPLAY_SIZE>
    getDisplayBuffer() const;

    void handleKeyUp(uint8_t chip8code);
    void handleKeyDown(uint8_t chip8code);

    Status step();
    void decrementTimers();
    void logState() {
        // SDL_Log("PC: %x", hardware.PC);
        // SDL_Log("SP: %x", hardware.SP);
        // std::string registers = "";
        //
        // for (int i = 0; i < Chip8Hardware::REGISTER_COUNT; i++) {
        //     registers += "V[" + std::to_string(i) +
        //                  "]=" + std::to_string(hardware.REGISTERS[i]) + " ";
        // }
        // SDL_Log("%s", registers.c_str());
    }

  private:
    void clearDisplay() {
        memset(&this->hardware.MEMORY[Chip8Hardware::DISPLAY_START], 0,
               Chip8Hardware::DISPLAY_SIZE);
    }
    void returnFromSubroutine();
    void callSubroutine(const int nnn);
    uint8_t getRandomByte() { return dist(gen); }

    Chip8Hardware hardware;
    std::mt19937 gen;
    std::uniform_int_distribution<uint8_t> dist;
};
