#ifndef DROOM_PALETTE_H
#define DROOM_PALETTE_H

#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>

class RGBColor {
 public:
  RGBColor() = default;

  RGBColor(int r, int g, int b) : r{r}, g{g}, b{b} {}

  RGBColor(const char rgb[3]) : r{rgb[0]}, g{rgb[1]}, b{rgb[2]} {}

  int r;
  int g;
  int b;
};

class Palette {
 public:
  [[nodiscard]] RGBColor operator[](int idx) const {
	assert(idx >= 0 && idx < COLORS_IN_PALETTE);
	return colors_[idx];
  }

  static constexpr int COLORS_IN_PALETTE = 256;
  static constexpr int BYTES_PER_PALETTE = COLORS_IN_PALETTE*3;

 private:
  std::array<RGBColor, COLORS_IN_PALETTE> colors_;

  friend std::istream &operator>>(std::istream &is, Palette &palette);
};

std::istream &operator>>(std::istream &is, Palette &palette);

class Colormap {
 public:
  [[nodiscard]] uint8_t operator[](int idx) const {
	assert(idx >= 0 && idx < Palette::COLORS_IN_PALETTE);
	return map_[idx];
  }

 private:
  std::array<uint8_t, Palette::COLORS_IN_PALETTE> map_;

  friend std::istream &operator>>(std::istream &is, Colormap &colormap);
};

std::istream &operator>>(std::istream &is, Colormap &colormap);

#endif //DROOM_PALETTE_H
