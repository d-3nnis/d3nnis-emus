#pragma once
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_scancode.h"
#include "common.hpp"
#include <array>
#include <bit>
#include <byteswap.h>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <expected>
#include <limits>
#include <stdexcept>
#include <string>
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
        static constexpr int DISPLAY_SIZE = 0x100;

        uint8_t MEMORY[MEMORY_SIZE] = {};
        uint8_t REGISTERS[REGISTER_COUNT] = {};
        uint8_t STACK[STACK_SIZE] = {};
        uint16_t KEY_STATE = 0;
        uint16_t PC = PROGRAM_START;
        uint16_t SP = 0;
        // only 12 bits
        uint16_t I = 0;
        uint8_t DELAY_TIMER = 0;
        uint8_t SOUND_TIMER = 0;
    };
    static constexpr int INSTRUCTIONS_PER_FRAME = 10;
    static constexpr int TARGET_FPS = 60;
    static constexpr int FRAME_TIME_MS = 1000 / TARGET_FPS;

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

    // chip8 sprites

    static constexpr int CHIP8_DISPLAY_WIDTH = 64;
    static constexpr int CHIP8_DISPLAY_HEIGHT = 32;

  public:
    Chip8() : memory() {
        assert(CHIP8_DISPLAY_HEIGHT * CHIP8_DISPLAY_WIDTH ==
               Chip8Hardware::DISPLAY_SIZE * BITS_PER_BYTE);
        memcpy(memory.MEMORY, Chip8Sprites::sprites.data(),
               Chip8Sprites::SPRITE_MEMORY_SIZE);
    }

    enum class Chip8Status {
        OK,
        ROM_OVERSIZED,
        UNRECOGNIZED_KEY,
        ERROR,
    };

    void startChip8Emu(void);

    Chip8Status loadProgram(std::vector<uint8_t> program) {
        auto size = program.size();
        if (size + Chip8Hardware::PROGRAM_START > Chip8Hardware::MEMORY_SIZE) {
            SDL_Log("ROM exceeds max size\n");
            return Chip8Status::ROM_OVERSIZED;
        }

        std::memcpy(&memory.MEMORY[Chip8Hardware::PROGRAM_START],
                    program.data(), size);
        return Chip8Status::OK;
    }

    void clearDisplay() {
        memset(&this->memory.MEMORY[Chip8Hardware::DISPLAY_START], 0,
               Chip8Hardware::DISPLAY_SIZE);
    }

    void returnFromSubroutine() {
        memory.SP -= 2;
        uint16_t storedSP =
            (memory.STACK[memory.SP] << 8 | memory.STACK[memory.SP + 1]);
        SDL_Log("Returning to %x\n", storedSP);
        memory.PC = storedSP;
        memory.PC += 2;
    }

    void callSubroutine(const int nnn) {
        memory.STACK[memory.SP] = (memory.PC >> 8);
        memory.STACK[memory.SP + 1] = memory.PC & 0xFF;
        memory.SP += 2;
        memory.PC = nnn;
    }

    void handleKeyUp(SDL_Scancode code) {
        auto result = mapSDLCode(code);
        if (result.has_value()) {
            memory.KEY_STATE &= ~(1 << result.value());
        } else {
            SDL_Log("UNRECOGNIZED_KEY");
        }
    }
    void handleKeyDown(SDL_Scancode code) {
        auto result = mapSDLCode(code);
        if (result.has_value()) {
            memory.KEY_STATE |= 1 << result.value();
        } else {
            SDL_Log("UNRECOGNIZED_KEY");
        }
    }

    // uint16_t

    std::expected<int, Chip8Status> mapSDLCode(SDL_Scancode code) {
        switch (code) {
        case SDL_SCANCODE_1:
            return 0x1;
        case SDL_SCANCODE_2:
            return 0x2;
        case SDL_SCANCODE_3:
            return 0x3;
        case SDL_SCANCODE_4:
            return 0xC;
        case SDL_SCANCODE_Q:
            return 0x4;
        case SDL_SCANCODE_W:
            return 0x5;
        case SDL_SCANCODE_E:
            return 0x6;
        case SDL_SCANCODE_R:
            return 0xD;
        case SDL_SCANCODE_A:
            return 0x7;
        case SDL_SCANCODE_S:
            return 0x8;
        case SDL_SCANCODE_D:
            return 0x9;
        case SDL_SCANCODE_F:
            return 0xE;
        case SDL_SCANCODE_Z:
            return 0xA;
        case SDL_SCANCODE_X:
            return 0x0;
        case SDL_SCANCODE_C:
            return 0xB;
        case SDL_SCANCODE_V:
            return 0xF;
        default:
            return std::unexpected(Chip8Status::UNRECOGNIZED_KEY);
        }
    }

    Chip8Status handleInstruction() {
        const uint16_t instruction =
            (memory.MEMORY[memory.PC] << 8 | memory.MEMORY[memory.PC + 1]);
        const int xRegisterIdx = (instruction & 0x0F00) >> 8;
        const int yRegisterIdx = (instruction & 0x00F0) >> 4;
        const int nnn = (instruction & 0x0FFF);
        const int n = (instruction & 0x000F);
        const int kk = (instruction & 0x00FF);

        SDL_Log("instruction: %x\n", instruction);
        logState();
        switch ((instruction & 0xF000) >> 12) {
        case 0x0: {
            switch (instruction) {
            case 0x00E0:
                clearDisplay();
                memory.PC += 2;
                break;
            case 0x00EE:
                returnFromSubroutine();
                break;
            }
            break;
        }
        case 0x1: {
            memory.PC = nnn;
            break;
        }
        case 0x2: {
            callSubroutine(nnn);
            break;
        }
        case 0x3: {
            int registerValue = memory.REGISTERS[xRegisterIdx];
            if (kk == registerValue) {
                memory.PC += 4;
            } else {
                memory.PC += 2;
            }
            break;
        }
        case 0x4: {
            int registerValue = memory.REGISTERS[xRegisterIdx];
            if (kk != registerValue) {
                memory.PC += 4;
            } else {
                memory.PC += 2;
            }
            break;
        }
        case 0x5:
            throw std::runtime_error("unhandled case\n");
            break;
        case 0x6: {
            uint8_t instructionValue =
                static_cast<uint8_t>(instruction & 0x00FF);
            memory.REGISTERS[xRegisterIdx] = instructionValue;
            memory.PC += 2;
            break;
        }
        case 0x7: {
            memory.REGISTERS[xRegisterIdx] += kk;
            memory.PC += 2;
            break;
        }
        case 0x8: {
            switch (instruction & 0xF) {
            case 0x0: {
                memory.REGISTERS[xRegisterIdx] = memory.REGISTERS[yRegisterIdx];
                break;
            }
            case 0x1: {
                memory.REGISTERS[xRegisterIdx] |=
                    memory.REGISTERS[yRegisterIdx];
                break;
            }
            case 0x2: {
                memory.REGISTERS[xRegisterIdx] &=
                    memory.REGISTERS[yRegisterIdx];
                break;
            }
            case 0x3: {
                memory.REGISTERS[xRegisterIdx] ^=
                    memory.REGISTERS[yRegisterIdx];
                break;
            }
            case 0x4: {
                int total = memory.REGISTERS[xRegisterIdx] +
                            memory.REGISTERS[yRegisterIdx];
                if (total > std::numeric_limits<uint8_t>::max()) {
                    memory.REGISTERS[0xF] = 1;
                } else {
                    memory.REGISTERS[0xF] = 0;
                }
                memory.REGISTERS[xRegisterIdx] = static_cast<uint8_t>(total);
                break;
            }
            case 0x5: {
                // TODO wrong?
                int result = memory.REGISTERS[xRegisterIdx] -
                             memory.REGISTERS[yRegisterIdx];
                if (result < 0) {
                    memory.REGISTERS[0xF] = 0;
                } else {
                    memory.REGISTERS[0xF] = 1;
                }
                memory.REGISTERS[xRegisterIdx] -=
                    memory.REGISTERS[yRegisterIdx];
                break;
            }
            case 0x6: {
                if ((memory.REGISTERS[xRegisterIdx] & 1) == 1) {
                    memory.REGISTERS[0xF] = 1;
                } else {
                    memory.REGISTERS[0xF] = 0;
                }
                memory.REGISTERS[xRegisterIdx] =
                    memory.REGISTERS[xRegisterIdx] >> 1;
                break;
            }
            case 0x7: {
                int result = memory.REGISTERS[yRegisterIdx] -
                             memory.REGISTERS[xRegisterIdx];
                if (result < 0) {
                    memory.REGISTERS[0xF] = 0;
                } else {
                    memory.REGISTERS[0xF] = 1;
                }
                memory.REGISTERS[xRegisterIdx] =
                    memory.REGISTERS[yRegisterIdx] -
                    memory.REGISTERS[xRegisterIdx];
                break;
            }
            case 0xE: {
                if (((memory.REGISTERS[xRegisterIdx] >> 7) & 1) == 1) {
                    memory.REGISTERS[0xF] = 1;
                } else {
                    memory.REGISTERS[0xF] = 0;
                }
                memory.REGISTERS[xRegisterIdx] = memory.REGISTERS[xRegisterIdx]
                                                 << 1;
                break;
            }
            default: {
                throw std::runtime_error("unhandled case\n");
                break;
            }
            }
            memory.PC += 2;
            break;
        }
        case 0x9:
            throw std::runtime_error("unhandled case\n");
            break;
        case 0xA: {
            memory.I = nnn;
            memory.PC += 2;
            break;
        }
        case 0xB:
            throw std::runtime_error("unhandled case\n");
            break;
        case 0xC:
            throw std::runtime_error("unhandled case\n");
            break;
        case 0xD: {
            int spriteHeight = n;
            memory.REGISTERS[0xF] = 0;
            auto xRegister = memory.REGISTERS[xRegisterIdx];
            auto yRegister = memory.REGISTERS[yRegisterIdx];
            for (int row = 0; row < spriteHeight; row++) {
                auto drawByte = memory.MEMORY[memory.I + row];
                for (int col = 0; col < BITS_PER_BYTE; col++) {
                    // iterate over bits
                    bool pixelOn = drawByte & (0x80 >> col);
                    if (pixelOn) {
                        int xPixel = (xRegister + col) % CHIP8_DISPLAY_WIDTH;
                        int yPixel = (yRegister + row) % CHIP8_DISPLAY_HEIGHT;
                        int byteIdx = (yPixel * CHIP8_DISPLAY_WIDTH + xPixel) /
                                      BITS_PER_BYTE;
                        int bitIdx =
                            7 - ((yPixel * CHIP8_DISPLAY_WIDTH + xPixel) %
                                 BITS_PER_BYTE);
                        if (memory.MEMORY[Chip8Hardware::DISPLAY_START +
                                          byteIdx] &
                            (1 << bitIdx)) {
                            memory.REGISTERS[0xF] = 1;
                        }
                        memory.MEMORY[Chip8Hardware::DISPLAY_START + byteIdx] ^=
                            (1 << bitIdx);
                    }
                }
            }
            memory.PC += 2;
            break;
        }
        case 0xE:
            throw std::runtime_error("unhandled case\n");

            break;
        case 0xF:
            switch (instruction & 0xFF) {
            case 0x0A: {
                int registerIdx = (instruction & 0x0F00) >> 8;
                if (!memory.KEY_STATE) {
                    break;
                }

                memory.REGISTERS[registerIdx] =
                    std::countr_zero(memory.KEY_STATE);
                memory.PC += 2;
                break;
            }
            case 0x15: {
                memory.DELAY_TIMER = memory.REGISTERS[xRegisterIdx];

                memory.PC += 2;
                break;
            }
            case 0x18: {
                memory.SOUND_TIMER = memory.REGISTERS[xRegisterIdx];
                memory.PC += 2;
                break;
            }
            case 0x1E: {
                memory.I += memory.REGISTERS[xRegisterIdx];
                memory.PC += 2;
                break;
            }
            case 0x55: {
                // TODO conflicting specs here
                memcpy(&memory.MEMORY[memory.I], &memory.REGISTERS[0],
                       xRegisterIdx + 1);
                memory.PC += 2;
                break;
            }
            default: {
                throw std::runtime_error("Unrecognized instruction:" +
                                         std::to_string(instruction) + "\n");
                break;
            }
            }
            break;
        default: {
            throw std::runtime_error("Unrecognized instruction:" +
                                     std::to_string(instruction) + "\n");
        }
        }
        return Chip8Status::OK;
    }

    void decrementTimers() {
        if (memory.DELAY_TIMER > 0) {
            memory.DELAY_TIMER--;
        }
        if (memory.SOUND_TIMER > 0) {
            memory.SOUND_TIMER--;
        }
    }

    Chip8Status nextCycle() {
        handleInstruction();
        return Chip8Status::OK;
    }

    void logState() {
        SDL_Log("PC: %x", memory.PC);
        SDL_Log("SP: %x", memory.SP);
        std::string registers = "";

        for (int i = 0; i < Chip8Hardware::REGISTER_COUNT; i++) {
            registers += "V[" + std::to_string(i) +
                         "]=" + std::to_string(memory.REGISTERS[i]) + " ";
        }
        SDL_Log("%s", registers.c_str());
    }

  private:
    Chip8Hardware memory;
};
