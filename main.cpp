#include "Application.h"

int main() {
  auto wad_path = std::filesystem::path{"/home/nikola/Dropbox/Privatno/software/games/Doom/DOOM.WAD"};

  DroomApplication application{std::string{"Droom"}, 800, 600, 320, 200, wad_path};
  application.main_loop();

  return 0;
}
