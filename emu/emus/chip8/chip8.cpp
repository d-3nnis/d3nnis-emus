#include "chip8.hpp"
#include "Chip8SDLDisplay.hpp"
#include <unistd.h>

bool AppLifecycleWatcher(void *userdata, SDL_Event *event) {
    return false;
}

void startChip8Emu(void) {
    Chip8SDLDisplay<CHIP8_DISPLAY_WIDTH, CHIP8_DISPLAY_HEIGHT> display;
    SDL_AddEventWatch(AppLifecycleWatcher, NULL);
    bool quit = false;
    while (!quit) {
        // THIS IS WRONG
        display.update(MEMORY, MEMORY_SIZE);
        SDL_Event evt;
        while (SDL_PollEvent(&evt)) {
            if (evt.type == SDL_EVENT_QUIT) {
                quit = 1;
            }
        }
    }
}
