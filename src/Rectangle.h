#pragma once
#include <SDL2/SDL.h>

class Rectangle {
    public:
        Rectangle(const SDL_Rect& Rect) : Rect{Rect} {}
        // render function
        void Render(SDL_Surface* Surface) const {
            SDL_FillRect(
                Surface,
                &Rect,
                SDL_MapRGB(Surface->format, 255, 0, 0)
            );
        }
    private:
        SDL_Rect Rect;
};