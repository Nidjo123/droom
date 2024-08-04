#ifndef DROOM_WAD_H
#define DROOM_WAD_H

#include <cstdint>
#include <filesystem>
#include <map>
#include <memory>
#include <vector>

#include "Color.h"

struct WadInfo {
  std::string wad_type;
  uint32_t num_lumps;
  uint32_t info_table_offset;
};

struct LumpInfo {
  uint32_t file_pos;
  uint32_t size;
  std::string name;
};

struct Thing {
  int16_t x;
  int16_t y;
  int16_t angle;
  int16_t type;
  int16_t flags;
};

struct Linedef {
  int16_t start_vertex;
  int16_t end_vertex;
  int16_t flags;
  int16_t special_type;
  int16_t sector_tag;
  int16_t front_sidedef;
  int16_t back_sidedef;
};

struct Sidedef {
  int16_t x_offset;
  int16_t y_offset;
  int8_t upper_tex_name[8];
  int8_t lower_tex_name[8];
  int8_t middle_tex_name[8];
  int16_t sector;
};

struct Vertex {
  int16_t x;
  int16_t y;
};

struct Seg {
  int16_t start_vertex;
  int16_t end_vertex;
  int16_t angle;
  int16_t linedef;
  int16_t direction;
  int16_t offset;
};

struct Subsector {
  int16_t seg_count;
  int16_t first_seg;
};

struct Node {
  int16_t x;
  int16_t y;
  int16_t x_offset;
  int16_t y_offset;
  int16_t right_bbox[4];
  int16_t left_bbox[4];
  int16_t right_child;
  int16_t left_child;
};

struct Sector {
  int16_t floor_height;
  int16_t ceiling_height;
  int8_t floor_tex_name[8];
  int8_t ceil_tex_name[8];
  int16_t light_level;
  int16_t special_type;
  int16_t tag;
};

struct Reject {
};

struct Blockmap {
  struct Header {
	int16_t x_origin;
	int16_t y_origin;
	int16_t num_cols;
	int16_t num_rows;
  } header;
  std::vector<int16_t> offsets;
};

struct Map {
  std::vector<Thing> things;
  std::vector<Linedef> linedefs;
  std::vector<Sidedef> sidedefs;
  std::vector<Vertex> vertexes;
  std::vector<Seg> segs;
  std::vector<Subsector> ssectors;
};

struct Picture {
  struct PictureFormat {
	int16_t width;
	int16_t height;
	int16_t left_offset;
	int16_t top_offset;
  } header_;

  struct Column {
	struct Post {
	  uint8_t top_offset;
	  std::vector<uint8_t> pixels;
	};
	std::vector<Post> posts;
  };

  std::vector<Column> columns_;

  friend std::istream &operator>>(std::istream &is, Picture &picture);
  friend Column read_picture_column(std::istream &is);
};

std::istream &operator>>(std::istream &is, Picture &picture);

class Wad {
 public:
  Wad() = default;

  static std::shared_ptr<Wad> from_file(std::filesystem::path &);

  Map get_map(std::string name) const {
	return maps_.at(name);
  }

  std::vector<std::string> get_map_names() const {
	std::vector<std::string> map_names;
	for (const auto &it : maps_) {
	  map_names.push_back(it.first);
	}
	return map_names;
  }

  const Picture &get_sprite(const std::string &name) {
	return sprites_[name];
  }

  const Picture &get_patch(const std::string &name) {
	return patches_[name];
  }

  [[nodiscard]] const Colormap &get_colormap(int idx) const {
	return colormaps_[idx];
  }

  [[nodiscard]] const Palette &get_palette(int idx) const {
	return palettes_[idx];
  }

 private:
  void load_from_stream(std::istream &);

  void load_header(std::istream &);

  void load_directory(std::istream &);

  template<typename T>
  static std::vector<T> load_lumps(std::istream &, const LumpInfo &);

  void load_lump_data(std::istream &is);

  WadInfo wad_info_;
  std::vector<LumpInfo> lump_infos_;
  std::vector<Palette> palettes_;
  std::vector<Colormap> colormaps_;
  std::map<std::string, Map> maps_;
  std::map<std::string, Picture> sprites_;
  std::map<std::string, Picture> patches_;
  std::string end_text;
};

bool is_map_name(const std::string &);

#endif //DROOM_WAD_H
