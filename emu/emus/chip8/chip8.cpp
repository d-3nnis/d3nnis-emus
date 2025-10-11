#include "chip8.hpp"
#include <bit>

Chip8::Status Chip8::loadProgram(std::vector<uint8_t> program) {
    auto size = program.size();
    if (size + Chip8Hardware::PROGRAM_START > Chip8Hardware::MEMORY_SIZE) {
        // SDL_Log("ROM exceeds max size\n");
        return Status::ROM_OVERSIZED;
    }

    std::memcpy(&hardware.MEMORY[Chip8Hardware::PROGRAM_START], program.data(),
                size);
    return Status::OK;
}

const std::span<const uint8_t, Chip8::Chip8Hardware::DISPLAY_SIZE>
Chip8::getDisplayBuffer() const {
    return std::span(hardware.MEMORY)
        .subspan<Chip8Hardware::DISPLAY_START, Chip8Hardware::DISPLAY_SIZE>();
}

void Chip8::returnFromSubroutine() {
    hardware.SP -= 2;
    uint16_t storedSP =
        (hardware.STACK[hardware.SP] << 8 | hardware.STACK[hardware.SP + 1]);
    hardware.PC = storedSP;
    hardware.PC += 2;
}

void Chip8::callSubroutine(const int nnn) {
    hardware.STACK[hardware.SP] = (hardware.PC >> 8);
    hardware.STACK[hardware.SP + 1] = hardware.PC & 0xFF;
    hardware.SP += 2;
    hardware.PC = nnn;
}

void Chip8::handleKeyUp(uint8_t chip8code) {
    if (chip8code < 0 || chip8code > 0xF) {
        return;
    }
    hardware.KEY_STATE &= ~(1 << chip8code);
}

void Chip8::handleKeyDown(uint8_t chip8code) {
    if (chip8code < 0 || chip8code > 0xF) {
        return;
    }
    hardware.KEY_STATE |= 1 << chip8code;
}

Chip8::Status Chip8::step() {
    const uint16_t instruction =
        (hardware.MEMORY[hardware.PC] << 8 | hardware.MEMORY[hardware.PC + 1]);
    const int xRegisterIdx = (instruction & 0x0F00) >> 8;
    const int yRegisterIdx = (instruction & 0x00F0) >> 4;
    const int nnn = (instruction & 0x0FFF);
    const int n = (instruction & 0x000F);
    const int kk = (instruction & 0x00FF);

    switch ((instruction & 0xF000) >> 12) {
    case 0x0: {
        switch (instruction) {
        case 0x00E0: {
            clearDisplay();
            hardware.PC += 2;
            break;
        }
        case 0x00EE: {
            returnFromSubroutine();
            break;
        }
        default: {
            return Status::INVALID_INSTRUCTION;
        }
        }
        break;
    }
    case 0x1: {
        hardware.PC = nnn;
        break;
    }
    case 0x2: {
        callSubroutine(nnn);
        break;
    }
    case 0x3: {
        int registerValue = hardware.REGISTERS[xRegisterIdx];
        if (kk == registerValue) {
            hardware.PC += 4;
        } else {
            hardware.PC += 2;
        }
        break;
    }
    case 0x4: {
        int registerValue = hardware.REGISTERS[xRegisterIdx];
        if (kk != registerValue) {
            hardware.PC += 4;
        } else {
            hardware.PC += 2;
        }
        break;
    }
    case 0x5: {
        int xRegister = hardware.REGISTERS[xRegisterIdx];
        int yRegister = hardware.REGISTERS[yRegisterIdx];
        if (xRegister == yRegister) {
            hardware.PC += 4;
        } else {
            hardware.PC += 2;
        }
        break;
    }
    case 0x6: {
        uint8_t instructionValue = static_cast<uint8_t>(instruction & 0x00FF);
        hardware.REGISTERS[xRegisterIdx] = instructionValue;
        hardware.PC += 2;
        break;
    }
    case 0x7: {
        hardware.REGISTERS[xRegisterIdx] += kk;
        hardware.PC += 2;
        break;
    }
    case 0x8: {
        switch (instruction & 0xF) {
        case 0x0: {
            hardware.REGISTERS[xRegisterIdx] = hardware.REGISTERS[yRegisterIdx];
            break;
        }
        case 0x1: {
            hardware.REGISTERS[xRegisterIdx] |=
                hardware.REGISTERS[yRegisterIdx];
            break;
        }
        case 0x2: {
            hardware.REGISTERS[xRegisterIdx] &=
                hardware.REGISTERS[yRegisterIdx];
            break;
        }
        case 0x3: {
            hardware.REGISTERS[xRegisterIdx] ^=
                hardware.REGISTERS[yRegisterIdx];
            break;
        }
        case 0x4: {
            int total = hardware.REGISTERS[xRegisterIdx] +
                        hardware.REGISTERS[yRegisterIdx];
            if (total > std::numeric_limits<uint8_t>::max()) {
                hardware.REGISTERS[0xF] = 1;
            } else {
                hardware.REGISTERS[0xF] = 0;
            }
            hardware.REGISTERS[xRegisterIdx] = static_cast<uint8_t>(total);
            break;
        }
        case 0x5: {
            // TODO wrong?
            int result = hardware.REGISTERS[xRegisterIdx] -
                         hardware.REGISTERS[yRegisterIdx];
            if (result < 0) {
                hardware.REGISTERS[0xF] = 0;
            } else {
                hardware.REGISTERS[0xF] = 1;
            }
            hardware.REGISTERS[xRegisterIdx] -=
                hardware.REGISTERS[yRegisterIdx];
            break;
        }
        case 0x6: {
            if ((hardware.REGISTERS[xRegisterIdx] & 1) == 1) {
                hardware.REGISTERS[0xF] = 1;
            } else {
                hardware.REGISTERS[0xF] = 0;
            }
            hardware.REGISTERS[xRegisterIdx] =
                hardware.REGISTERS[xRegisterIdx] >> 1;
            break;
        }
        case 0x7: {
            int result = hardware.REGISTERS[yRegisterIdx] -
                         hardware.REGISTERS[xRegisterIdx];
            if (result < 0) {
                hardware.REGISTERS[0xF] = 0;
            } else {
                hardware.REGISTERS[0xF] = 1;
            }
            hardware.REGISTERS[xRegisterIdx] =
                hardware.REGISTERS[yRegisterIdx] -
                hardware.REGISTERS[xRegisterIdx];
            break;
        }
        case 0xE: {
            if (((hardware.REGISTERS[xRegisterIdx] >> 7) & 1) == 1) {
                hardware.REGISTERS[0xF] = 1;
            } else {
                hardware.REGISTERS[0xF] = 0;
            }
            hardware.REGISTERS[xRegisterIdx] = hardware.REGISTERS[xRegisterIdx]
                                               << 1;
            break;
        }
        default: {
            return Status::INVALID_INSTRUCTION;
        }
        }
        hardware.PC += 2;
        break;
    }
    case 0x9: {
        int xRegister = hardware.REGISTERS[xRegisterIdx];
        int yRegister = hardware.REGISTERS[yRegisterIdx];
        if (xRegister != yRegister) {
            hardware.PC += 4;
        } else {
            hardware.PC += 2;
        }
        break;
    }
    case 0xA: {
        hardware.I = nnn;
        hardware.PC += 2;
        break;
    }
    case 0xB: {
        int registerValue = hardware.REGISTERS[0];
        hardware.PC = nnn + registerValue;
        break;
    }
    case 0xC: {
        hardware.REGISTERS[xRegisterIdx] =
            getRandomByte() & static_cast<uint8_t>(kk);
        hardware.PC += 2;
        break;
    }
    case 0xD: {
        int spriteHeight = n;
        hardware.REGISTERS[0xF] = 0;
        auto xRegister = hardware.REGISTERS[xRegisterIdx];
        auto yRegister = hardware.REGISTERS[yRegisterIdx];
        for (int row = 0; row < spriteHeight; row++) {
            auto drawByte = hardware.MEMORY[hardware.I + row];
            for (int col = 0; col < BITS_PER_BYTE; col++) {
                // iterate over bits
                bool pixelOn = drawByte & (0x80 >> col);
                if (pixelOn) {
                    int xPixel = (xRegister + col) % CHIP8_DISPLAY_WIDTH;
                    int yPixel = (yRegister + row) % CHIP8_DISPLAY_HEIGHT;
                    int byteIdx =
                        (yPixel * CHIP8_DISPLAY_WIDTH + xPixel) / BITS_PER_BYTE;
                    int bitIdx = 7 - ((yPixel * CHIP8_DISPLAY_WIDTH + xPixel) %
                                      BITS_PER_BYTE);
                    if (hardware
                            .MEMORY[Chip8Hardware::DISPLAY_START + byteIdx] &
                        (1 << bitIdx)) {
                        hardware.REGISTERS[0xF] = 1;
                    }
                    hardware.MEMORY[Chip8Hardware::DISPLAY_START + byteIdx] ^=
                        (1 << bitIdx);
                }
            }
        }
        hardware.PC += 2;
        break;
    }
    case 0xE: {
        uint8_t key = hardware.REGISTERS[xRegisterIdx] & 0xF;
        switch (instruction & 0xFF) {
        case 0x9E: {
            if (((1 << key) & hardware.KEY_STATE) != 0) {
                hardware.PC += 4;
            } else {
                hardware.PC += 2;
            }
            break;
        }
        case 0xA1: {
            if (((1 << key) & hardware.KEY_STATE) == 0) {
                hardware.PC += 4;
            } else {
                hardware.PC += 2;
            }
            break;
        }
        }
        break;
    }
    case 0xF:
        switch (instruction & 0xFF) {
        case 0x07: {
            hardware.REGISTERS[xRegisterIdx] = hardware.DELAY_TIMER;
            hardware.PC += 2;
            break;
        }
        case 0x0A: {
            static bool waitingForKeyUp = false;
            static uint8_t keyPressed = 0;

            if (!waitingForKeyUp && hardware.KEY_STATE) {
                keyPressed = std::countr_zero(hardware.KEY_STATE);
                waitingForKeyUp = true;
            } else if (waitingForKeyUp && !hardware.KEY_STATE) {
                waitingForKeyUp = false;
                hardware.REGISTERS[xRegisterIdx] = keyPressed;
                hardware.PC += 2;
            }
            break;
        }
        case 0x15: {
            hardware.DELAY_TIMER = hardware.REGISTERS[xRegisterIdx];
            hardware.PC += 2;
            break;
        }
        case 0x18: {
            hardware.SOUND_TIMER = hardware.REGISTERS[xRegisterIdx];
            hardware.PC += 2;
            break;
        }
        case 0x1E: {
            hardware.I += hardware.REGISTERS[xRegisterIdx];
            hardware.PC += 2;
            break;
        }
        case 0x29: {
            hardware.I = Chip8Hardware::FONT_SET_START +
                         (hardware.REGISTERS[xRegisterIdx] *
                          Chip8Sprites::SPRITE_HEIGHT);
            hardware.PC += 2;
            break;
        }
        case 0x33: {
            int value = hardware.REGISTERS[xRegisterIdx];
            int hundreds = value / 100;
            int tens = (value / 10) % 10;
            int ones = value % 10;
            hardware.MEMORY[hardware.I] = hundreds;
            hardware.MEMORY[hardware.I + 1] = tens;
            hardware.MEMORY[hardware.I + 2] = ones;
            hardware.PC += 2;
            break;
        }
        case 0x55: {
            // TODO conflicting specs here
            memcpy(&hardware.MEMORY[hardware.I], &hardware.REGISTERS[0],
                   xRegisterIdx + 1);
            hardware.PC += 2;
            break;
        }
        case 0x65: {
            // TODO conflicting specs here
            memcpy(&hardware.REGISTERS[0], &hardware.MEMORY[hardware.I],
                   xRegisterIdx + 1);
            hardware.PC += 2;
            break;
        }
        default: {
            return Status::INVALID_INSTRUCTION;
        }
        }
        break;
    default: {
        return Status::INVALID_INSTRUCTION;
    }
    }
    return Status::OK;
}

void Chip8::decrementTimers() {
    if (hardware.DELAY_TIMER > 0) {
        hardware.DELAY_TIMER--;
    }
    if (hardware.SOUND_TIMER > 0) {
        hardware.SOUND_TIMER--;
    }
}
