#include <cstdlib>

#include "Application.h"

SDLApplication::SDLApplication() {
  SDL_Init(SDL_INIT_EVERYTHING);
}

SDLApplication::~SDLApplication() {
  SDL_Quit();
}

void SDLApplication::main_loop() {
  bool running = true;
  auto prev_tick_time = SDL_GetPerformanceCounter();
  while (running) {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
	  switch (event.type) {
		case SDL_QUIT:running = false;
		  break;
		default:break;
	  }
	  process_event(event);
	}

	const auto tick_time = SDL_GetPerformanceCounter();
	const float delta =
		static_cast<float>(tick_time - prev_tick_time)/static_cast<float>(SDL_GetPerformanceFrequency());

	tick(delta);
	render();

	SDL_Delay(1);

	prev_tick_time = tick_time;
  }
}

void SDLApplication::process_event(SDL_Event &event) {
}

DroomApplication::DroomApplication(std::string title, int window_width, int window_height, int screen_width,
								   int screen_height, std::filesystem::path &wad_path)
	: screen_{screen_width, screen_height} {
  window_ = std::unique_ptr<SDL_Window, std::function<decltype(SDL_DestroyWindow)>>(
	  SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height,
					   SDL_WINDOW_RESIZABLE),
	  SDL_DestroyWindow);
  if (!window_) {
	SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Failed to create window: %s", SDL_GetError());
	std::exit(1);
  }

  renderer_ = std::unique_ptr<SDL_Renderer, std::function<decltype(SDL_DestroyRenderer)>>(
	  SDL_CreateRenderer(window_.get(), -1, 0),
	  SDL_DestroyRenderer);
  if (!renderer_) {
	SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Failed to create renderer: %s", SDL_GetError());
	std::exit(1);
  }

  wad_ = Wad::from_file(wad_path);
}

void DroomApplication::tick(float delta) {
}

void DroomApplication::render() {
  screen_.clear(RGBAColor::BLACK);
  screen_.draw_line(0, 0, screen_.get_width(), screen_.get_height(), RGBAColor::WHITE);
  screen_.draw_line(screen_.get_width(), 0, 0, screen_.get_height(), RGBAColor::WHITE);

  const auto palette = wad_->get_palette(0);
  auto sprite = wad_->get_sprite("CYBRE1");
  for (auto x = 0; x < sprite.columns_.size(); x++) {
	for (const auto &post : sprite.columns_[x].posts) {
	  for (auto y = 0; y < post.pixels.size(); y++) {
		const auto color = palette[post.pixels[y]];
		const auto xx = sprite.columns_.size() - 1 - x;
		screen_.draw_pixel(xx, post.top_offset + y, RGBAColor(color.r, color.g, color.b));
	  }
	}
  }

  screen_.present();
  present_screen();
}

int DroomApplication::width() const {
  int width_;
  SDL_GetRendererOutputSize(renderer_.get(), &width_, nullptr);
  return width_;
}

int DroomApplication::height() const {
  int height_;
  SDL_GetRendererOutputSize(renderer_.get(), nullptr, &height_);
  return height_;
}

void DroomApplication::present_screen() {
  SDL_Renderer *renderer = renderer_.get();
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(renderer);

  auto screen_texture = screen_.get_texture(renderer);
  SDL_assert(screen_texture);
  if (SDL_RenderCopy(renderer, screen_texture.get(), nullptr, nullptr)) {
	SDL_Log("Error copying texture: %s", SDL_GetError());
  }

  SDL_RenderPresent(renderer);
}
