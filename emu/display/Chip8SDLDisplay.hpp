#include "SDL3/SDL_init.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"
#include "SDL3/SDL_video.h"
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <sys/types.h>

enum class DisplayStatus {
    DISPLAY_OK,
    DISPLAY_ERROR,
};

template<std::size_t DisplayPixelWidth, std::size_t DisplayPixelHeight>
class Chip8SDLDisplay {
  public:
    Chip8SDLDisplay() {
        // TODO use some kind of singleton manager?
        if (SDL_WasInit(SDL_INIT_VIDEO)) {
            throw std::runtime_error("SDL already initialized");
        }

        // SDL_Init(SDL_INIT_VIDEO);
        SDL_InitSubSystem(SDL_INIT_VIDEO);
        window = SDL_CreateWindow("Chip8Emu", DisplayPixelWidth * 10, DisplayPixelHeight * 10,
                                  SDL_WINDOW_RESIZABLE);
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
            // free(window);
        }
        if (renderer) {
            // free(renderer);
        }
    }

    DisplayStatus update(const uint8_t *chip8DisplayBuf, const std::size_t size) {
        // displayBuf is an array of bytes where each pixel is a bit.
        // SDL Texture needs an array of uint32_t where each pixel is 0xFFFFFFFF or 0x00000000
        uint32_t displayBuf[DisplayPixelWidth * DisplayPixelHeight] = {0xFFFFFFFF};

        for(std::size_t i = 0; i < size; i++) {
            constexpr int BITS_PER_BYTE = 8;
            for(std::size_t bit = 0; bit < BITS_PER_BYTE; bit++) {
                // std::size_t pixelIndex = i * BITS_PER_BYTE + bit;
                // if(pixelIndex >= DisplayPixelWidth * DisplayPixelHeight) {
                //     break;
                // }
                // bool pixelOn = (displayBuf[i] >> (7 - bit)) & 1;
                // ((uint32_t*)displayBuf)[pixelIndex] = pixelOn ? 0xFFFFFFFF : 0x00000000;
            }
        }
        SDL_UpdateTexture(texture, nullptr, displayBuf,
                          DisplayPixelWidth);
        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
        return DisplayStatus::DISPLAY_OK;
    }

  private:
    // std::array<uint32_t, > Chip8SDLDisplayBuffer;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
};
