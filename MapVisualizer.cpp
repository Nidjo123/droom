#include "Application.h"

#include <algorithm>
#include <cstdlib>
#include <filesystem>


class MapVisualizer : public DroomApplication {
public:
    MapVisualizer(std::string title, int width, int height, std::filesystem::path &wad_path)
            : DroomApplication{title, width, height, wad_path}, map_names_{wad_->get_map_names()} {
        std::sort(map_names_.begin(), map_names_.end());
    }

protected:
    void process_event(SDL_Event &event) override {
        if (event.type == SDL_KEYDOWN) {
            int old_idx = map_idx_;
            if (event.key.keysym.sym == SDLK_LEFT) {
                map_idx_ = std::max(0, map_idx_ - 1);
            } else if (event.key.keysym.sym == SDLK_RIGHT) {
                map_idx_ = std::min(static_cast<int>(map_names_.size()) - 1, map_idx_ + 1);
            }

            if (map_idx_ != old_idx) {
                SDL_Log("Drawing map %s", map_names_[map_idx_].c_str());
            }
        }
    }

    void tick(float delta) override {}

    void render() override {
        const auto map = wad_->get_map(map_names_[map_idx_]);
        auto vertexes = map.vertexes;
        const auto x_range = std::minmax_element(vertexes.begin(), vertexes.end(),
                                                 [](const auto &v1, const auto &v2) { return v1.x < v2.x; });
        const auto y_range = std::minmax_element(vertexes.begin(), vertexes.end(),
                                                 [](const auto &v1, const auto &v2) { return v1.y < v2.y; });
        const auto min_x = x_range.first->x;
        const auto max_x = x_range.second->x;
        const auto min_y = y_range.first->y;
        const auto max_y = y_range.second->y;

        const float x_scale = width() / static_cast<float>(max_x - min_x);
        const float y_scale = height() / static_cast<float>(max_y - min_y);
        const float scale = std::min(x_scale, y_scale);
        for (auto &vertex: vertexes) {
            vertex.x -= min_x;
            vertex.y -= min_y;
            vertex.x *= scale;
            vertex.y *= scale;
        }

        SDL_SetRenderDrawColor(renderer_.get(), 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer_.get());

        SDL_SetRenderDrawColor(renderer_.get(), 200, 255, 200, SDL_ALPHA_OPAQUE);
        for (const auto &linedef: map.linedefs) {
            const auto &start_vertex = vertexes[linedef.start_vertex];
            const auto &end_vertex = vertexes[linedef.end_vertex];
            SDL_RenderDrawLine(renderer_.get(), start_vertex.x, start_vertex.y, end_vertex.x, end_vertex.y);
        }

        SDL_SetRenderDrawColor(renderer_.get(), 200, 100, 200, SDL_ALPHA_OPAQUE);
        for (const auto &vertex: vertexes) {
            SDL_RenderDrawPoint(renderer_.get(), vertex.x, vertex.y);
        }

        SDL_RenderPresent(renderer_.get());
    }

private:
    std::vector<std::string> map_names_;
    int map_idx_ = 0;
};


int main(int argc, char *argv[]) {
    if (argc != 2) {
        SDL_Log("Usage: mapvis path_to_wad");
        std::exit(1);
    }

    auto wad_path = std::filesystem::path{argv[1]};
    if (!is_regular_file(wad_path)) {
        SDL_Log("WAD file must exist!");
        std::exit(1);
    }

    MapVisualizer application{std::string{"mapvis"}, 800, 600, wad_path};
    application.main_loop();

    return 0;
}
