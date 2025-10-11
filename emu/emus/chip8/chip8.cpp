#include "chip8.hpp"
#include "Chip8SDLDisplay.hpp"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_timer.h"
#include <chrono>
#include <unistd.h>

bool AppLifecycleWatcher(void *userdata, SDL_Event *event) { return false; }

void Chip8::startChip8Emu(void) {
    Chip8SDLDisplay<CHIP8_DISPLAY_WIDTH, CHIP8_DISPLAY_HEIGHT> display;
    SDL_AddEventWatch(AppLifecycleWatcher, NULL);

    // for (int i = Chip8Hardware::DISPLAY_START;
    //      i < Chip8Hardware::DISPLAY_START + (Chip8Hardware::DISPLAY_SIZE / 2);
    //      i++) {
    //     this->memory.MEMORY[i] = 0xFF;
    // }

    bool quit = false;
    while (!quit) {
        auto frame_start = std::chrono::steady_clock::now();
        SDL_Event evt;
        while (SDL_PollEvent(&evt)) {
            SDL_Log("Event: %d", evt.type);
            if (evt.type == SDL_EVENT_QUIT) {
                quit = 1;
            } else if (evt.type == SDL_EVENT_KEY_DOWN) {
                handleKeyDown(evt.key.scancode);
            } else if (evt.type == SDL_EVENT_KEY_UP) {
                handleKeyUp(evt.key.scancode);
            }
        }
        for (int i = 0; i < INSTRUCTIONS_PER_FRAME; i++) {
            nextCycle();
        }
        decrementTimers();
        display.update(&this->memory.MEMORY[Chip8Hardware::DISPLAY_START],
                       Chip8Hardware::DISPLAY_SIZE);
        auto frame_end = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                           frame_end - frame_start)
                           .count();
        if (elapsed < FRAME_TIME_MS) {
            SDL_Delay(FRAME_TIME_MS - elapsed);
        }
    }
}
