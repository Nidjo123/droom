#include <cstdlib>
#include "Screen.h"

const RGBAColor RGBAColor::BLACK = RGBAColor(0, 0, 0);
const RGBAColor RGBAColor::WHITE = RGBAColor(255, 255, 255);

Screen::Screen(int width, int height) : width_{width}, height_{height} {
    surface_ = std::unique_ptr<SDL_Surface, std::function<decltype(SDL_FreeSurface)>>(
            SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0),
            SDL_FreeSurface);
    if (!surface_) {
        SDL_Log("Failed to create surface: %s", SDL_GetError());
        std::exit(1);
    }

    renderer_ = std::unique_ptr<SDL_Renderer, std::function<decltype(SDL_DestroyRenderer)>>(
            SDL_CreateSoftwareRenderer(surface_.get()),
            SDL_DestroyRenderer);
    if (!renderer_) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        std::exit(1);
    }
}

void Screen::draw_pixel(int x, int y, RGBAColor color) {
    set_draw_color(color);
    SDL_RenderDrawPoint(renderer_.get(), x, y);
}

void Screen::draw_line(int x0, int y0, int x1, int y1, RGBAColor color) {
    set_draw_color(color);
    SDL_RenderDrawLine(renderer_.get(), x0, y0, x1, y1);
}

int Screen::get_width() const {
    return width_;
}

int Screen::get_height() const {
    return height_;
}

void Screen::clear(RGBAColor color) {
    set_draw_color(color);
    if (SDL_MUSTLOCK(surface_.get())) {
        ;
    }
    if (SDL_RenderClear(renderer_.get())) {
        SDL_Log("Error clearing surface: %s", SDL_GetError());
    }
}

void Screen::set_draw_color(RGBAColor color) {
    SDL_SetRenderDrawColor(renderer_.get(), color.r, color.g, color.b, color.a);
}

std::shared_ptr<SDL_Texture> Screen::get_texture(SDL_Renderer *renderer) const {
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface_.get());
    SDL_assert(texture);
    return {texture, SDL_DestroyTexture};
}

void Screen::present() {
    SDL_RenderPresent(renderer_.get());
}
