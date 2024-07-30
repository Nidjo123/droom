#include "Wad.h"

#include <SDL.h>

#include <fstream>
#include <regex>


template<typename T>
std::istream &operator>>(std::istream &is, T &lump) {
    is.read(reinterpret_cast<char *>(&lump), sizeof(lump));
    SDL_assert(is && is.gcount() == sizeof(lump));
    return is;
}


std::istream &operator>>(std::istream &is, WadInfo &wad_info) {
    constexpr int MAGIC_BYTES = 4;
    char magic[MAGIC_BYTES + 1] = {0};
    is.read(magic, MAGIC_BYTES);
    SDL_assert(is && is.gcount() == MAGIC_BYTES);
    wad_info.wad_type = magic;
    is.read(reinterpret_cast<char *>(&wad_info.num_lumps), sizeof(wad_info.num_lumps));
    SDL_assert(is && is.gcount() == sizeof(wad_info.num_lumps));
    is.read(reinterpret_cast<char *>(&wad_info.info_table_offset), sizeof(wad_info.info_table_offset));
    SDL_assert(is && is.gcount() == sizeof(wad_info.info_table_offset));
    return is;
}

std::istream &operator>>(std::istream &is, LumpInfo &lump_info) {
    is.read(reinterpret_cast<char *>(&lump_info.file_pos), sizeof(lump_info.file_pos));
    SDL_assert(is && is.gcount() == sizeof(lump_info.file_pos));
    is.read(reinterpret_cast<char *>(&lump_info.size), sizeof(lump_info.size));
    SDL_assert(is && is.gcount() == sizeof(lump_info.size));
    constexpr int NAME_BYTES = 8;
    char name[NAME_BYTES + 1] = {0};
    is.read(name, NAME_BYTES);
    lump_info.name = name;
    SDL_assert(is && is.gcount() == NAME_BYTES);
    return is;
}

std::shared_ptr<Wad> Wad::from_file(std::filesystem::path &path) {
    std::ifstream ifs{path, std::ios::binary};
    if (!ifs) {
        return nullptr;
    }

    auto wad = std::make_shared<Wad>();
    wad->load_from_stream(ifs);

    ifs.close();

    return wad;
}

void Wad::load_from_stream(std::istream &is) {
    load_header(is);
    load_directory(is);
    load_maps(is);
}

void Wad::load_header(std::istream &is) {
    is >> wad_info;
}

void Wad::load_directory(std::istream &is) {
    is.seekg(wad_info.info_table_offset);
    for (auto i = 0; i < wad_info.num_lumps; i++) {
        LumpInfo lump_info;
        is >> lump_info;
        lump_infos.push_back(lump_info);
        SDL_Log("Found lump %s", lump_info.name.c_str());
    }
}

void Wad::load_maps(std::istream &is) {
    for (auto i = 0; i < lump_infos.size(); i++) {
        const auto lump_info = lump_infos[i];
        if (is_map_name(lump_info.name)) {
            SDL_Log("Loading map %s", lump_info.name.c_str());
            auto things = load_lumps<Thing>(is, lump_infos[i + 1]);
            SDL_Log("Loaded %zu things", things.size());
            auto linedefs = load_lumps<Linedef>(is, lump_infos[i + 2]);
            SDL_Log("Loaded %zu linedefs", linedefs.size());
            auto sidedefs = load_lumps<Sidedef>(is, lump_infos[i + 3]);
            SDL_Log("Loaded %zu sidedefs", sidedefs.size());
            auto vertexes = load_lumps<Vertex>(is, lump_infos[i + 4]);
            SDL_Log("Loaded %zu vertexes", vertexes.size());
            auto segs = load_lumps<Seg>(is, lump_infos[i + 5]);
            SDL_Log("Loaded %zu segs", segs.size());
            auto ssectors = load_lumps<Subsector>(is, lump_infos[i + 6]);
            SDL_Log("Loaded %zu ssectors", ssectors.size());
            auto nodes = load_lumps<Node>(is, lump_infos[i + 7]);
            SDL_Log("Loaded %zu nodes", nodes.size());
            auto sectors = load_lumps<Sector>(is, lump_infos[i + 8]);
            SDL_Log("Loaded %zu sectors", sectors.size());

            maps[std::string{lump_info.name}] = {.things=things,
                    .linedefs=linedefs,
                    .sidedefs=sidedefs,
                    .vertexes=vertexes,
                    .segs=segs,
                    .ssectors=ssectors};
            SDL_Log("Loading map with name %s", lump_info.name.c_str());
        }
    }
}

template<typename T>
std::vector<T> Wad::load_lumps(std::istream &is, const LumpInfo &lump_info) {
    std::vector<T> lumps;
    is.seekg(lump_info.file_pos);
    for (auto i = 0; i < lump_info.size / sizeof(T); i++) {
        T item{};
        is.read(reinterpret_cast<char *>(&item), sizeof(item));
        SDL_assert(is.gcount() == sizeof(T));
        lumps.push_back(item);
    }
    return lumps;
}

const std::regex MAP_NAME_REGEX{R"(^E(\d+)M(\d+)$)"};

bool is_map_name(const std::string &s) {
    return std::regex_match(s, MAP_NAME_REGEX);
}
