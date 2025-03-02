#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define SCREEN_FPS 144
#define DELTA_TIME_SEC (1.0f / SCREEN_FPS)
#define DELTA_TIME_MSEC ((Uint32)floorf(DELTA_TIME_SEC * 1000))

#define MARKER_SIZE 20.0f

#define AMBIENCE_COLOR 0xB9D0E9FF
#define BACKGROUND_COLOR 0x000000FF
#define LINE_COLOR 0xDA2C38FF
#define RECT_COLOR 0x87C38FFF

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

int main(int argc, char* argv[]) {
	check_sdl_code(SDL_Init(SDL_INIT_VIDEO));

	SDL_Window* const window =
		check_sdl_ptr(SDL_CreateWindow("Bezier Curves", 400, 400, SCREEN_WIDTH,
									   SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE));

	SDL_Renderer* const renderer =
		check_sdl_ptr(SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED));
	check_sdl_code(
		SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT));

	const Vec2 begin = vec2(0.0f, 0.0f);
	const Vec2 end = vec2(SCREEN_WIDTH, SCREEN_HEIGHT);
	float p = 0.0f;

	int quit = 0;
	float t = 0.0f;
	while (!quit) {
		SDL_Event event = {0};

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					quit = 1;
					break;
				case SDL_KEYDOWN: {
					switch (event.key.keysym.sym) {
						case (SDLK_9): {
							quit = 1;
							exit(0);
						} break;
						case (SDLK_UP): {
							p += 0.1f;
						} break;
						case (SDLK_DOWN): {
							p -= 0.1f;
						} break;
					}
				}
			}
		}

		check_sdl_code(
			SDL_SetRenderDrawColor(renderer, HEX_COLOR(BACKGROUND_COLOR)));
		check_sdl_code(SDL_RenderClear(renderer));

		render_marker(renderer, begin, LINE_COLOR);
		render_marker(renderer, end, LINE_COLOR);
		render_marker(renderer, lerp_vec2(begin, end, (sin(t) + 1.0f) / 2),
					  RECT_COLOR);

		SDL_RenderPresent(renderer);
		SDL_Delay(DELTA_TIME_MSEC);
		t += DELTA_TIME_SEC;
	}

	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	return 0;
}
