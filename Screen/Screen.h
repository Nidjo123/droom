#ifndef DROOM_SCREEN_H
#define DROOM_SCREEN_H

#include <functional>
#include <memory>

#include <SDL.h>

class RGBAColor {
 public:
  RGBAColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a = SDL_ALPHA_OPAQUE) : r{r}, g{g}, b{b}, a{a} {}

  Uint8 r;
  Uint8 g;
  Uint8 b;
  Uint8 a;

  static const RGBAColor BLACK;
  static const RGBAColor WHITE;
};

class Screen {
 public:
  Screen(int width, int height);

  void clear(RGBAColor color);

  void present();

  void draw_pixel(int x, int y, RGBAColor color);

  void draw_line(int x0, int y0, int x1, int y1, RGBAColor color);

  [[nodiscard]] std::shared_ptr<SDL_Texture> get_texture(SDL_Renderer *renderer) const;

  int get_width() const;

  int get_height() const;

 private:
  void set_draw_color(RGBAColor color);

  int width_;
  int height_;
  std::unique_ptr<SDL_Surface, std::function<decltype(SDL_FreeSurface)>> surface_;
  std::unique_ptr<SDL_Renderer, std::function<decltype(SDL_DestroyRenderer)>> renderer_;
};

#endif //DROOM_SCREEN_H
