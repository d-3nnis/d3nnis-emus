#pragma once
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"
#include "SDL3/SDL_video.h"
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <sys/types.h>

#include "common.hpp"

enum class DisplayStatus {
    DISPLAY_OK,
    DISPLAY_ERROR,
};

template <std::size_t DisplayPixelWidth, std::size_t DisplayPixelHeight>
class Chip8SDLDisplay {
  public:
    Chip8SDLDisplay() {
        // TODO use some kind of singleton manager?
        if (SDL_WasInit(SDL_INIT_VIDEO)) {
            throw std::runtime_error("SDL already initialized");
        }

        // SDL_Init(SDL_INIT_VIDEO);
        SDL_InitSubSystem(SDL_INIT_VIDEO);
        window =
            SDL_CreateWindow("Chip8Emu", DisplayPixelWidth * 10,
                             DisplayPixelHeight * 10, SDL_WINDOW_RESIZABLE);
        renderer = SDL_CreateRenderer(window, nullptr);
        // Maybe not streaming?
        texture = SDL_CreateTexture(
            renderer, SDL_PixelFormat::SDL_PIXELFORMAT_RGBA8888,
            SDL_TextureAccess::SDL_TEXTUREACCESS_STREAMING, DisplayPixelWidth,
            DisplayPixelHeight);
        SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
    }

    ~Chip8SDLDisplay() {
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        if (window) {
            SDL_DestroyWindow(window);
        }
        if (renderer) {
            SDL_DestroyRenderer(renderer);
        }
        if (texture) {
            SDL_DestroyTexture(texture);
        }
    }

    DisplayStatus update(const uint8_t *chip8DisplayBuf,
                         const std::size_t size) {
        // displayBuf is an array of bytes where each pixel is a bit.
        // SDL Texture needs an array of uint32_t where each pixel is 0xFFFFFFFF
        // or 0x00000000
        std::array<uint32_t, DisplayPixelWidth * DisplayPixelHeight> displayBuf;
        assert(
            (size * BITS_PER_BYTE >= DisplayPixelWidth * DisplayPixelHeight));

        for (std::size_t i = 0; i < size; i++) {
            for (std::size_t bit = 0; bit < BITS_PER_BYTE; bit++) {
                bool bitOn = (chip8DisplayBuf[i] >> (7 - bit)) & 1;
                displayBuf.at(i * BITS_PER_BYTE + bit) =
                    bitOn ? 0xFFFFFFFF : 0x00000000;
            }
        }
        SDL_UpdateTexture(texture, nullptr, displayBuf.data(),
                          DisplayPixelWidth * sizeof(uint32_t));
        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
        return DisplayStatus::DISPLAY_OK;
    }

  private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
};
