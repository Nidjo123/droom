#include "Palette.h"

std::istream &operator>>(std::istream &is, Palette &palette) {
  for (auto i = 0; i < Palette::COLORS_IN_PALETTE; i++) {
	char rgb[3];
	is.read(rgb, 3);
	assert(is.gcount()==3);
	palette.colors_[i] = RGBColor(rgb);
  }
  return is;
}
