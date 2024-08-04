#include "Wad.h"

#include <SDL.h>

#include <fstream>
#include <regex>

// some basic assumptions
static_assert(sizeof(uint8_t)==sizeof(char));
static_assert(sizeof(unsigned char)==sizeof(char));
static_assert(sizeof(uint8_t)==sizeof(int8_t));

template<typename T>
std::istream &operator>>(std::istream &is, T &lump) {
  is.read(reinterpret_cast<char *>(&lump), sizeof(lump));
  SDL_assert(is && is.gcount()==sizeof(lump));
  return is;
}

std::istream &operator>>(std::istream &is, WadInfo &wad_info) {
  constexpr int MAGIC_BYTES = 4;
  char magic[MAGIC_BYTES + 1] = {0};
  is.read(magic, MAGIC_BYTES);
  SDL_assert(is && is.gcount()==MAGIC_BYTES);
  wad_info.wad_type = magic;
  is.read(reinterpret_cast<char *>(&wad_info.num_lumps), sizeof(wad_info.num_lumps));
  SDL_assert(is && is.gcount()==sizeof(wad_info.num_lumps));
  is.read(reinterpret_cast<char *>(&wad_info.info_table_offset), sizeof(wad_info.info_table_offset));
  SDL_assert(is && is.gcount()==sizeof(wad_info.info_table_offset));
  return is;
}

std::istream &operator>>(std::istream &is, LumpInfo &lump_info) {
  is.read(reinterpret_cast<char *>(&lump_info.file_pos), sizeof(lump_info.file_pos));
  SDL_assert(is && is.gcount()==sizeof(lump_info.file_pos));
  is.read(reinterpret_cast<char *>(&lump_info.size), sizeof(lump_info.size));
  SDL_assert(is && is.gcount()==sizeof(lump_info.size));
  constexpr int NAME_BYTES = 8;
  char name[NAME_BYTES + 1] = {0};
  is.read(name, NAME_BYTES);
  lump_info.name = name;
  SDL_assert(is && is.gcount()==NAME_BYTES);
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
  load_lump_data(is);
}

void Wad::load_header(std::istream &is) {
  is >> wad_info_;
}

void Wad::load_directory(std::istream &is) {
  is.seekg(wad_info_.info_table_offset);
  for (auto i = 0; i < wad_info_.num_lumps; i++) {
	LumpInfo lump_info;
	is >> lump_info;
	lump_infos_.push_back(lump_info);
	SDL_Log("Found lump %s", lump_info.name.c_str());
  }
}

void Wad::load_lump_data(std::istream &is) {
  bool reading_sprites = false;
  bool reading_flats = false;

  for (auto i = 0; i < lump_infos_.size(); i++) {
	const auto lump_info = lump_infos_[i];
	if (is_map_name(lump_info.name)) {
	  SDL_Log("Loading map %s", lump_info.name.c_str());
	  auto things = load_lumps<Thing>(is, lump_infos_[i + 1]);
	  SDL_Log("Loaded %zu things", things.size());
	  auto linedefs = load_lumps<Linedef>(is, lump_infos_[i + 2]);
	  SDL_Log("Loaded %zu linedefs", linedefs.size());
	  auto sidedefs = load_lumps<Sidedef>(is, lump_infos_[i + 3]);
	  SDL_Log("Loaded %zu sidedefs", sidedefs.size());
	  auto vertexes = load_lumps<Vertex>(is, lump_infos_[i + 4]);
	  SDL_Log("Loaded %zu vertexes", vertexes.size());
	  auto segs = load_lumps<Seg>(is, lump_infos_[i + 5]);
	  SDL_Log("Loaded %zu segs", segs.size());
	  auto ssectors = load_lumps<Subsector>(is, lump_infos_[i + 6]);
	  SDL_Log("Loaded %zu ssectors", ssectors.size());
	  auto nodes = load_lumps<Node>(is, lump_infos_[i + 7]);
	  SDL_Log("Loaded %zu nodes", nodes.size());
	  auto sectors = load_lumps<Sector>(is, lump_infos_[i + 8]);
	  SDL_Log("Loaded %zu sectors", sectors.size());

	  maps_[std::string{lump_info.name}] = {.things=things,
		  .linedefs=linedefs,
		  .sidedefs=sidedefs,
		  .vertexes=vertexes,
		  .segs=segs,
		  .ssectors=ssectors};
	  SDL_Log("Loading map with name %s", lump_info.name.c_str());
	} else if (lump_info.name=="PLAYPAL") {
	  is.seekg(lump_info.file_pos);
	  const auto num_palettes = lump_info.size/Palette::BYTES_PER_PALETTE;
	  for (auto pal_idx = 0; pal_idx < num_palettes; pal_idx++) {
		Palette palette{};
		is >> palette;
		palettes_.push_back(palette);
	  }
	  SDL_Log("Loaded %zu palettes", palettes_.size());
	} else if (lump_info.name=="COLORMAP") {
	  is.seekg(lump_info.file_pos);
	  const auto num_colormaps = lump_info.size/Palette::COLORS_IN_PALETTE;
	  for (auto colmap_idx = 0; colmap_idx < num_colormaps; colmap_idx++) {
		Colormap colormap{};
		is >> colormap;
		colormaps_.push_back(colormap);
	  }
	  SDL_Log("Loaded %zu colormaps", colormaps_.size());
	} else if (lump_info.name=="ENDOOM") {
	  is.seekg(lump_info.file_pos);
	  for (auto s_idx = 0; s_idx < lump_info.size; s_idx += 2) {
		char s[2];
		is.read(s, 2);
		end_text += s[0];
	  }
	} else if (lump_info.name=="S_START") {
	  reading_sprites = true;
	  SDL_assert(lump_info.size==0);
	} else if (lump_info.name=="S_END") {
	  reading_sprites = false;
	  SDL_assert(lump_info.size==0);
	} else if (lump_info.name=="F_START") {
	  reading_flats = true;
	  SDL_assert(lump_info.size==0);
	} else if (lump_info.name=="F_END") {
	  reading_flats = false;
	  SDL_assert(lump_info.size==0);
	} else if (reading_sprites) {
	  is.seekg(lump_info.file_pos);
	  Picture picture;
	  is >> picture;
	  sprites_[lump_info.name] = picture;
	  SDL_Log("Read sprite %s", lump_info.name.c_str());
	}
  }

  SDL_assert(!reading_sprites);
  SDL_assert(!reading_flats);
}

template<typename T>
std::vector<T> Wad::load_lumps(std::istream &is, const LumpInfo &lump_info) {
  std::vector<T> lumps;
  is.seekg(lump_info.file_pos);
  for (auto i = 0; i < lump_info.size/sizeof(T); i++) {
	T item{};
	is.read(reinterpret_cast<char *>(&item), sizeof(item));
	SDL_assert(is.gcount()==sizeof(T));
	lumps.push_back(item);
  }
  return lumps;
}

const std::regex MAP_NAME_REGEX{R"(^(E(\d+)M(\d+))|(MAP(\d+))$)"};

bool is_map_name(const std::string &s) {
  return std::regex_match(s, MAP_NAME_REGEX);
}

Picture::Column read_picture_column(std::istream &is) {
  std::vector<uint8_t> posts;
  uint8_t start_row = 0;
  uint8_t num_pixels = 0;
  while (true) {
	is.read(reinterpret_cast<char *>(&start_row), 1);
	assert(is.gcount()==sizeof(start_row));
	if (start_row==0xFF) {
	  break;
	}
	is.read(reinterpret_cast<char *>(&num_pixels), 1);
	assert(is.gcount()==sizeof(num_pixels));
	posts.push_back(start_row);
	posts.push_back(num_pixels);
	const auto bytes_to_read = num_pixels + 2;
	std::vector<uint8_t> post(bytes_to_read);
	is.read(reinterpret_cast<char *>(&post[0]), bytes_to_read);
	assert(is.gcount()==bytes_to_read);
	std::copy(post.begin(), post.end(), std::back_inserter(posts));
  }
  return {posts};
}

std::istream &operator>>(std::istream &is, Picture &picture) {
  const auto lump_pos = is.tellg();
  is.read(reinterpret_cast<char *>(&picture.header_), sizeof(Picture::PictureFormat));
  SDL_assert(is.gcount()==sizeof(Picture::PictureFormat));
  const auto cols = picture.header_.width;
  for (auto col_idx = 0; col_idx < cols; col_idx++) {
	uint32_t col_offset;
	is.read(reinterpret_cast<char *>(&col_offset), sizeof(col_offset));
	SDL_assert(is.gcount()==sizeof(col_offset));
	const auto next_pos = is.tellg();
	is.seekg(lump_pos + std::streamoff(col_offset));
	picture.columns_.push_back(read_picture_column(is));
	is.seekg(next_pos);
  }
  return is;
}
