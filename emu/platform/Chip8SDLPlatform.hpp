#pragma once
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#include "chip8.hpp"
#include <cassert>
#include <cstdint>
#include <expected>
#include <span>
#include <sys/types.h>
#include <vector>

enum class DisplayStatus {
    DISPLAY_OK,
    DISPLAY_ERROR,
};

class Chip8SDLPlatform {
  public:
    struct Config {
        const int displayPixelWidth;
        const int displayPixelHeight;
        int displayScale;

        int instructionsPerFrame;
    };

    Chip8SDLPlatform(const Config &config);

    ~Chip8SDLPlatform();

    void run(Chip8 &emulator);

    DisplayStatus render(std::span<const uint8_t> chip8DisplayBuf);

    void handleEvents(Chip8 &emulator, bool &quit);
    std::expected<int, Chip8::Status> mapSDLToChip8(SDL_Scancode code);

  private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;

    const Config config;

    std::vector<uint32_t> pixelBuffer;
    const int displayWidth;
    const int displayHeight;
};
