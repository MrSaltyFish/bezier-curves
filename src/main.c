#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define SCREEN_FPS 144
#define DELTA_TIME_SEC (1.0f / SCREEN_FPS)
#define DELTA_TIME_MSEC ((Uint32)floorf(DELTA_TIME_SEC * 1000))

#define MARKER_SIZE 10.0f

#define MARKER_COLOR 0xB9D0E9FF
#define BACKGROUND_COLOR 0x000000FF
#define LINE_COLOR 0xDA2C38FF
#define INTERPOLATOR_COLOR 0x87C38FFF

#define HEX_COLOR(hex)                                    \
	((hex) >> (3 * 8)) & 0xFF, ((hex) >> (2 * 8)) & 0xFF, \
		((hex) >> (1 * 8)) & 0xFF, ((hex) >> (0 * 8)) & 0xFF

// ====== Check Functions ======
int check_sdl_code(int code) {
	if (code < 0) {
		fprintf(stderr, "SDL error: %s\n", SDL_GetError());
		exit(1);
	}
	return code;
}

void* check_sdl_ptr(void* ptr) {
	if (ptr == NULL) {
		fprintf(stderr, "SDL error: %s\n", SDL_GetError());
		exit(1);
	}
	return ptr;
}

// ====== Arithmetic Functions ======
float lerp_f(float a, float b, float p) { return a + (b - a) * p; }

// ====== Vector Functions ======
typedef struct {
	float x;
	float y;
} Vec2;

Vec2 vec2(float x, float y) { return (Vec2){x, y}; }
Vec2 vec2_add(Vec2 a, Vec2 b) { return vec2(a.x + b.x, a.y + b.y); }
Vec2 vec2_sub(Vec2 a, Vec2 b) { return vec2(a.x - b.x, a.y - b.y); }
Vec2 vec2_scale(Vec2 a, float p) { return vec2(a.x * p, a.y * p); }
Vec2 lerp_vec2(Vec2 a, Vec2 b, float p) {
	return vec2_add(a, vec2_scale(vec2_sub(b, a), p));
}

// ====== Rendering Functions ======
void render_line(SDL_Renderer* renderer, Vec2 begin, Vec2 end, uint32_t color) {
	check_sdl_code(SDL_SetRenderDrawColor(renderer, HEX_COLOR(color)));
	check_sdl_code(
		SDL_RenderDrawLineF(renderer, begin.x, begin.y, end.x, end.y));
}

// // Same function, just floorfed int values instead. No difference in output.
// void render_line(SDL_Renderer* renderer, Vec2 begin, Vec2 end, uint32_t
// color) { 	check_sdl_code(SDL_SetRenderDrawColor(renderer,
// HEX_COLOR(color))); 	check_sdl_code(SDL_RenderDrawLine(renderer,
// (int)floorf(begin.x), (int)floorf(begin.y), (int)floorf(end.x),
// 									  (int)floorf(end.y)));
// }

void fill_rect(SDL_Renderer* renderer, Vec2 pos, Vec2 size, uint32_t color) {
	check_sdl_code(SDL_SetRenderDrawColor(renderer, HEX_COLOR(color)));
	const SDL_Rect rect = {.x = pos.x, .y = pos.y, .w = size.x, .h = size.y};
	check_sdl_code(SDL_RenderFillRect(renderer, &rect));
}

void render_marker(SDL_Renderer* renderer, Vec2 pos, uint32_t color) {
	const Vec2 size = vec2(MARKER_SIZE, MARKER_SIZE);
	fill_rect(renderer, vec2_sub(pos, vec2_scale(size, 0.5f)), size, color);
}

#define PS_CAPACITY 256

Vec2 ps[PS_CAPACITY];
size_t ps_count = 0;

int main(int argc, char* argv[]) {
	check_sdl_code(SDL_Init(SDL_INIT_VIDEO));

	SDL_Window* const window =
		check_sdl_ptr(SDL_CreateWindow("Bezier Curves", 400, 400, SCREEN_WIDTH,
									   SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE));

	SDL_Renderer* const renderer =
		check_sdl_ptr(SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE));
	check_sdl_code(
		SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT));

	int a = 5;
	int quit = 0;
	float t = 0.0f;
	while (!quit) {
		SDL_Event event = {0};

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					quit = 1;
					break;

				// Handle the keyboard inputs
				case SDL_KEYDOWN: {
					switch (event.key.keysym.sym) {
						case (SDLK_9): {
							quit = 1;
							exit(0);
						} break;
						case (SDLK_UP): {
							a++;
						} break;
						case (SDLK_DOWN): {
							a--;
						} break;
					}
				} break;

				// Handle the keyboard inputs
				case SDL_MOUSEBUTTONDOWN: {
					switch (event.button.button) {
						case (SDL_BUTTON_LEFT): {
							ps[((ps_count++) % PS_CAPACITY)] =
								// ps[ps_count++] =
								vec2(event.button.x, event.button.y);
						} break;
					}
				} break;
			}
		}
		// Clear the rendering screen
		check_sdl_code(
			SDL_SetRenderDrawColor(renderer, HEX_COLOR(BACKGROUND_COLOR)));
		check_sdl_code(SDL_RenderClear(renderer));
		// Add what to render

		for (size_t i = 0; ps_count > 0 && i < ps_count; i++) {
			render_marker(renderer, ps[i], MARKER_COLOR);
		}

		for (size_t i = 0; ps_count > 0 && i < ps_count - 1; i++) {
			render_marker(renderer,
						  lerp_vec2(ps[i], ps[i + 1], ((sin(t) + 1.0f) * 0.5f)),
						  INTERPOLATOR_COLOR);
		}

		// Present the screen, add to time
		SDL_RenderPresent(renderer);
		SDL_Delay(DELTA_TIME_MSEC);
		t += DELTA_TIME_SEC;
	}

	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	return 0;
}
