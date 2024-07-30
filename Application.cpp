#include <cstdlib>

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
            process_event(event);
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

void SDLApplication::process_event(SDL_Event &event) {
}

DroomApplication::DroomApplication(std::string title, int width, int height, std::filesystem::path &wad_path) {
    window_ = std::unique_ptr<SDL_Window, std::function<decltype(SDL_DestroyWindow)>>(
            SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0),
            SDL_DestroyWindow);
    if (!window_) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Failed to create window: %s", SDL_GetError());
        std::exit(1);
    }
    renderer_ = std::unique_ptr<SDL_Renderer, std::function<decltype(SDL_DestroyRenderer)>>(
            SDL_CreateRenderer(window_.get(), -1, 0),
            SDL_DestroyRenderer);
    if (!renderer_) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Failed to create renderer: %s", SDL_GetError());
        std::exit(1);
    }
    wad_ = Wad::from_file(wad_path);
}

void DroomApplication::tick(float delta) {
}

void DroomApplication::render() {
    SDL_SetRenderDrawColor(renderer_.get(), 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer_.get());

    SDL_SetRenderDrawColor(renderer_.get(), 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(renderer_.get(), 0, 0, width(), height());
    SDL_RenderDrawLine(renderer_.get(), width(), 0, 0, height());
    SDL_RenderPresent(renderer_.get());
}

int DroomApplication::width() const {
    int width_;
    SDL_GetRendererOutputSize(renderer_.get(), &width_, nullptr);
    return width_;
}

int DroomApplication::height() const {
    int height_;
    SDL_GetRendererOutputSize(renderer_.get(), nullptr, &height_);
    return height_;
}
