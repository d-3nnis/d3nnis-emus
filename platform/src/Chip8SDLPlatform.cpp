#include "SDL3/SDL_init.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"
#include "SDL3/SDL_timer.h"
#include "SDL3/SDL_video.h"
#include <Chip8SDLPlatform.hpp>
#include <cassert>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <span>
#include <stdexcept>
#include <sys/types.h>
#include <vector>

#include "chip8.hpp"
#include "common.hpp"

Chip8SDLPlatform::~Chip8SDLPlatform() {
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
}

Chip8SDLPlatform::Chip8SDLPlatform(const Config &config) : config(config) {
    // TODO use some kind of singleton manager?
    if (SDL_WasInit(SDL_INIT_VIDEO)) {
        throw std::runtime_error("SDL already initialized");
    }

    SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

    window = SDL_CreateWindow(
        "CHIP-8 Emulator", Chip8::getDisplayWidth() * config.displayScale,
        Chip8::getDisplayHeight() * config.displayScale, SDL_WINDOW_RESIZABLE);

    renderer = SDL_CreateRenderer(window, nullptr);

    // Maybe not streaming?
    texture =
        SDL_CreateTexture(renderer, SDL_PixelFormat::SDL_PIXELFORMAT_RGBA8888,
                          SDL_TextureAccess::SDL_TEXTUREACCESS_STREAMING,
                          Chip8::getDisplayWidth(), Chip8::getDisplayHeight());

    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

    pixelBuffer.resize(Chip8::getDisplayWidth() * Chip8::getDisplayHeight());
}

DisplayStatus
Chip8SDLPlatform::render(std::span<const uint8_t> chip8DisplayBuf) {
    // displayBuf is an array of bytes where each pixel is a bit.
    // SDL Texture needs an array of uint32_t where each pixel is 0xFFFFFFFF
    // or 0x00000000
    // assert((size * BITS_PER_BYTE >= DisplayPixelWidth * DisplayPixelHeight));

    for (std::size_t i = 0; i < pixelBuffer.size(); i++) {
        int byteIdx = i / BITS_PER_BYTE;
        int bitIdx = 7 - (i % BITS_PER_BYTE);
        bool bitOn = (chip8DisplayBuf[byteIdx] >> bitIdx) & 1;
        pixelBuffer.at(i) = bitOn ? 0xFFFFFFFF : 0x00000000;
    }

    SDL_UpdateTexture(texture, nullptr, pixelBuffer.data(),
                      Chip8::getDisplayWidth() * sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
    return DisplayStatus::DISPLAY_OK;
}

void Chip8SDLPlatform::run(Chip8 &emulator) {
    bool quit = false;

    while (!quit) {
        auto frameStart = std::chrono::steady_clock::now();

        handleEvents(emulator, quit);

        for (int i = 0; i < config.instructionsPerFrame; i++) {
            auto status = emulator.step();
            if (status != Chip8::Status::OK) {
                // TODO update error handling
                SDL_Log("Emulator error: %d", static_cast<int>(status));
            }
        }

        emulator.decrementTimers();
        render(emulator.getDisplayBuffer());
        if (emulator.shouldBeep()) {
            audio.play();
        } else {
            audio.stop();
        }

        auto frameEnd = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                           frameEnd - frameStart)
                           .count();

        if (elapsed < Chip8::FRAME_TIME_MS) {
            SDL_Delay(Chip8::FRAME_TIME_MS - elapsed);
        }
    }
}
void Chip8SDLPlatform::handleEvents(Chip8 &emulator, bool &quit) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_EVENT_QUIT:
            quit = true;
            break;

        case SDL_EVENT_KEY_DOWN: {
            if (auto key = mapSDLToChip8(event.key.scancode)) {
                emulator.handleKeyDown(key.value());
            }
            break;
        }

        case SDL_EVENT_KEY_UP: {
            if (auto key = mapSDLToChip8(event.key.scancode)) {
                emulator.handleKeyUp(key.value());
            }
            break;
        }
        }
    }
}

std::expected<int, Chip8::Status>
Chip8SDLPlatform::mapSDLToChip8(SDL_Scancode code) {
    // CHIP-8 keypad:     Keyboard:
    // 1 2 3 C            1 2 3 4
    // 4 5 6 D            Q W E R
    // 7 8 9 E            A S D F
    // A 0 B F            Z X C V

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
        return std::unexpected(Chip8::Status::UNRECOGNIZED_KEY);
    }
}
