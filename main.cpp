#include <functional>
#include <memory>

#include "Application.h"
#include "WAD/Wad.h"

const int WIDTH = 800;
const int HEIGHT = 600;

int main() {
    DroomApplication application{std::string{"Droom"}, WIDTH, HEIGHT};
    application.main_loop();

    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window *window = SDL_CreateWindow("Droom", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    if (!window) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Failed to create window_: %s\n", SDL_GetError());
        return -1;
    }

    auto path = std::filesystem::path{"/home/nikola/Dropbox/Privatno/software/games/Doom/DOOM.WAD"};
    auto wad = Wad::from_file(path);
    SDL_assert(wad);


    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
