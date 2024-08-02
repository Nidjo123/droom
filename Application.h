#ifndef DROOM_APPLICATION_H
#define DROOM_APPLICATION_H

#include <filesystem>
#include <functional>
#include <memory>

#include <SDL.h>

#include "WAD/Wad.h"
#include "Screen/Screen.h"

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

  virtual void process_event(SDL_Event &event);

  void main_loop() override;
};

class DroomApplication : public SDLApplication {
 public:
  DroomApplication(std::string title, int window_width, int window_height, int screen_width, int screen_height,
				   std::filesystem::path &wad_path);

  int width() const;

  int height() const;

 protected:
  void tick(float delta) override;

  void render() override;

  void present_screen();

 protected:
  std::unique_ptr<SDL_Window, std::function<decltype(SDL_DestroyWindow)>> window_;
  std::unique_ptr<SDL_Renderer, std::function<decltype(SDL_DestroyRenderer)>> renderer_;
  Screen screen_;
  std::shared_ptr<Wad> wad_;
};

#endif //DROOM_APPLICATION_H
