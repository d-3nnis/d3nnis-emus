#include "SDL3/SDL_init.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#include <memory>

struct sdl_deleter {
    void operator()(SDL_Window *p) const { SDL_DestroyWindow(p); }
    void operator()(SDL_Renderer *p) const { SDL_DestroyRenderer(p); }
};

class SDLDisplay {
  public:
    SDLDisplay() {}
    ~SDLDisplay() {}

    using SDLWindowPtr = std::unique_ptr<SDL_Window, sdl_deleter>;
    using SDLRendererPtr = std::unique_ptr<SDL_Renderer, sdl_deleter>;

  private:
    void initDisplay(int displayHeight, int displayWidth, std::string &name) {
        SDL_Init(SDL_INIT_VIDEO);
        std::string sdlName = name + "_window";
        SDL_Window *win = SDL_CreateWindow(
            "Chip8Eumu", displayWidth, displayHeight, SDL_WINDOW_RESIZABLE);
        sdlName = name + "_renderer";
        SDL_Renderer *renderer = SDL_CreateRenderer(win, sdlName.c_str());
        SDL_Texture *texture = SDL_CreateTexture(
            renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
            displayWidth, displayHeight);
    }

    SDLWindowPtr window;
    SDLRendererPtr renderer;
};
