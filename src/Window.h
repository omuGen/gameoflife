#pragma once
#include <iostream>
#include <SDL2/SDL.h>

class Window {
    public:
        Window(){
            SDLWindow = SDL_CreateWindow(
                "Game of Life",
                SDL_WINDOWPOS_CENTERED,
                SDL_WINDOWPOS_CENTERED,
                1024,
                768,
                0
            );
        }

        SDL_Surface* GetSurface() const {
            return SDL_GetWindowSurface(SDLWindow);
        }

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        ~Window() {
            if (SDLWindow && SDL_WasInit(SDL_INIT_VIDEO)) {
                SDL_DestroyWindow(SDLWindow);
            } else {
                std::cout << "Skipping SDL_DestroyWindow\n";
            }
        }

    private:
        SDL_Window* SDLWindow{nullptr};
};
