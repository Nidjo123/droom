#include "Application.h"


SDLApplication::SDLApplication() {
    SDL_Init(SDL_INIT_EVERYTHING);
}

SDLApplication::~SDLApplication() {
    SDL_Quit();
}

void SDLApplication::main_loop() {
    bool running = true;
    auto prev_tick_time = SDL_GetPerformanceCounter();
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                default:
                    break;
            }
        }

        const auto tick_time = SDL_GetPerformanceCounter();
        const float delta =
                static_cast<float>(tick_time - prev_tick_time) / static_cast<float>(SDL_GetPerformanceFrequency());

        tick(delta);
        render();

        SDL_Delay(1);

        prev_tick_time = tick_time;
    }
}

DroomApplication::DroomApplication(std::string title, int width, int height) : width_{width}, height_{height} {
    window_ = std::unique_ptr<SDL_Window, std::function<decltype(SDL_DestroyWindow)>>(
            SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0),
            SDL_DestroyWindow);
    renderer_ = std::unique_ptr<SDL_Renderer, std::function<decltype(SDL_DestroyRenderer)>>(
            SDL_CreateRenderer(window_.get(), -1, 0),
            SDL_DestroyRenderer);
}

void DroomApplication::tick(float delta) {

}

void DroomApplication::render() {

}


