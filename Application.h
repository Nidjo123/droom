#ifndef DROOM_APPLICATION_H
#define DROOM_APPLICATION_H

#include <functional>
#include <memory>

#include <SDL.h>


class Application {
public:
    virtual void main_loop() = 0;

protected:
    virtual void tick(float delta) = 0;

    virtual void render() = 0;
};


class SDLApplication : public Application {
public:
    SDLApplication();

    virtual ~SDLApplication();

    void main_loop() override;
};

class DroomApplication : public SDLApplication {
public:
    DroomApplication(std::string title, int width, int height);

protected:
    void tick(float delta) override;

    void render() override;

private:
    const int width_;
    const int height_;

    std::unique_ptr<SDL_Window, std::function<decltype(SDL_DestroyWindow)>> window_;
    std::unique_ptr<SDL_Renderer, std::function<decltype(SDL_DestroyRenderer)>> renderer_;
};


#endif //DROOM_APPLICATION_H
