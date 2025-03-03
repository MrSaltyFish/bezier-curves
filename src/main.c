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

#define BACKGROUND_COLOR 0x121212FF
#define LINE_COLOR 0xDA2C38FF

#define AMBIENCE_COLOR 0xB9D0E9FF

#define WHITE_COLOR 0xFFFFFFFF
#define RED_COLOR 0xFF0000FF
#define BLUE_COLOR 0x0000FFFF
#define GREEN_COLOR 0x15FE15FF

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

// ======== Points ========
#define PS_CAPACITY 4

Vec2 ps[PS_CAPACITY];
size_t ps_count = 0;
int ps_selected = -1;

int ps_at(Vec2 pos) {
	const Vec2 ps_size = vec2(MARKER_SIZE, MARKER_SIZE);
	for (size_t i = 0; i < ps_count; i++) {
		const Vec2 ps_begin = vec2_sub(ps[i], vec2_scale(ps_size, 0.5f));
		const Vec2 ps_end = vec2_add(ps_begin, ps_size);

		if (ps_begin.x <= pos.x && pos.x <= ps_end.x && ps_begin.y <= pos.y &&
			pos.y <= ps_end.y) {
			return (int)i;
		}
	}
	return -1;
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

Vec2 bezier_sample(Vec2 a, Vec2 b, Vec2 c, Vec2 d, float p) {
	const Vec2 ab = lerp_vec2(a, b, p);
	const Vec2 bc = lerp_vec2(b, c, p);
	const Vec2 cd = lerp_vec2(c, d, p);
	const Vec2 abc = lerp_vec2(ab, bc, p);
	const Vec2 bcd = lerp_vec2(bc, cd, p);
	const Vec2 abcd = lerp_vec2(abc, bcd, p);
	return abcd;
}

void render_bezier_markers(SDL_Renderer* renderer, Vec2 a, Vec2 b, Vec2 c,
						   Vec2 d, float s, uint32_t color) {
	for (float p = 0.0f; p <= 1.0f; p += s) {
		render_marker(renderer, bezier_sample(a, b, c, d, p), color);
	}
}

void render_bezier_curve(SDL_Renderer* renderer, Vec2 a, Vec2 b, Vec2 c, Vec2 d,
						 float s, uint32_t color) {
	for (float p = 0.0f; p + s <= 1.0f; p += s) {
		const Vec2 line_begin = bezier_sample(a, b, c, d, p);
		const Vec2 line_end = bezier_sample(a, b, c, d, p + s);
		render_line(renderer, line_begin, line_end, color);
	}
}

// ======== EXPERIMENTAL ========

Vec2 bezier_sample_scalable(Vec2* points, size_t degree, float p) {
	Vec2 samples[degree + 1];

	for (size_t i = 0; i <= degree; i++) {
		samples[i] = points[i];
	}

	if (ps_count <= degree) {
		return vec2(0.0f, 0.0f);
	}

	for (size_t i = 1; i <= degree; i++) {
		for (size_t j = 0; j <= degree - i; j++) {
			samples[j] = lerp_vec2(samples[j], samples[j + 1], p);
		}
	}
	return samples[0];
}

void render_bezier_curve_scalable(SDL_Renderer* renderer, Vec2* points,
								  int degree, float s, uint32_t color) {
	for (float p = 0.0f; p + s <= 1.0f; p += s) {
		const Vec2 line_begin = bezier_sample_scalable(points, degree, p);
		const Vec2 line_end = bezier_sample_scalable(points, degree, p + s);
		render_line(renderer, line_begin, line_end, color);
	}
}

// ======== MAIN ========
int main(int argc, char* argv[]) {
	check_sdl_code(SDL_Init(SDL_INIT_VIDEO));

	SDL_Window* const window =
		check_sdl_ptr(SDL_CreateWindow("Bezier Curves", 400, 400, SCREEN_WIDTH,
									   SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE));

	SDL_Renderer* const renderer =
		check_sdl_ptr(SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED));
	check_sdl_code(
		SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT));

	int quit = 0;
	int markers = 1;
	float t = 0.0f;
	float bezier_sample_step = 0.01f;
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
						case (SDLK_F1): {
							if (markers == 1) {
								markers = 0;
							} else {
								markers = 1;
							};
						} break;
						case (SDLK_F2): {
							if (bezier_sample_step > 0.1f) {
								bezier_sample_step -= 0.1f;
							};
						} break;
						case (SDLK_F3): {
							if (bezier_sample_step < 0.999f) {
								bezier_sample_step += 0.1f;
							};
						} break;
					}
				} break;

				// Handle the keyboard inputs
				case SDL_MOUSEBUTTONDOWN: {
					switch (event.button.button) {
						case (SDL_BUTTON_LEFT): {
							const Vec2 mouse_pos =
								vec2(event.button.x, event.button.y);
							if (ps_count < 4) {
								ps[((ps_count++) % PS_CAPACITY)] =
									// ps[ps_count++] =
									mouse_pos;
							} else {
								ps_selected = ps_at(mouse_pos);
							}
						} break;
					}
				} break;

				case SDL_MOUSEBUTTONUP: {
					switch (event.button.button) {
						case (SDL_BUTTON_LEFT): {
							ps_selected = -1;
						} break;
					}
				} break;

				case SDL_MOUSEWHEEL: {
					if (event.wheel.y > 0) {
						bezier_sample_step =
							fminf(bezier_sample_step + 0.001f, 0.999f);
					} else if (event.wheel.y < 0) {
						bezier_sample_step =
							fmaxf(bezier_sample_step - 0.001f, 0.001f);
					}
				} break;

				case SDL_MOUSEMOTION: {
					const Vec2 mouse_pos = vec2(event.motion.x, event.motion.y);

					if (ps_selected > -1) {
						ps[ps_selected] = mouse_pos;
					}

				} break;
			}
		}
		// Clear the rendering screen
		check_sdl_code(
			SDL_SetRenderDrawColor(renderer, HEX_COLOR(BACKGROUND_COLOR)));
		check_sdl_code(SDL_RenderClear(renderer));
		// Add what to render

		float p = ((sin(t) + 1.0f) * 0.5f);

		if (ps_count == 3) {
			render_line(renderer, ps[0], ps[1], RED_COLOR);
			render_line(renderer, ps[1], ps[2], RED_COLOR);
			// render_bezier_markers(renderer, ps[0], ps[1], ps[2], ps[3],
			// 0.01f, 					  GREEN_COLOR);
			// render_bezier_curve(renderer, ps[0], ps[1], ps[2], ps[3], 0.01f,
			// GREEN_COLOR);
			render_bezier_curve_scalable(renderer, ps, 3, 0.01f, GREEN_COLOR);
		}

		if (ps_count >= 4) {
			if (markers) {
				render_bezier_markers(renderer, ps[0], ps[1], ps[2], ps[3],
									  bezier_sample_step, GREEN_COLOR);
			} else {
				render_bezier_curve(renderer, ps[0], ps[1], ps[2], ps[3],
									bezier_sample_step, GREEN_COLOR);
				// render_bezier_curve_scalable(renderer, ps, 4, step_size,
				// 							 GREEN_COLOR);
			}

			// render_bezier_markers(renderer, ps[0], ps[1], ps[2], ps[3],
			// 0.01f, 					  GREEN_COLOR);
			// render_bezier_curve(renderer, ps[0], ps[1], ps[2], ps[3], 0.01f,
			// GREEN_COLOR);
			// render_bezier_curve_scalable(renderer, ps, 4, BEZIER_SAMPLE_STEP,
			// 							 GREEN_COLOR);
			render_line(renderer, ps[0], ps[1], RED_COLOR);
			render_line(renderer, ps[2], ps[3], RED_COLOR);
		}

		for (size_t i = 0; i < ps_count; i++) {
			render_marker(renderer, ps[i], RED_COLOR);
		}

		// Present the screen, add to time
		SDL_RenderPresent(renderer);
		SDL_Delay(DELTA_TIME_MSEC);
		t += DELTA_TIME_SEC;
		if (t > 1000.0f) t = 0.0f;
	}

	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	return 0;
}
