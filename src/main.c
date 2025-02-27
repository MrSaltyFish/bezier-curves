#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define AMBIENCE_COLOR 0xB9D0E9FF
#define BACKGROUND_COLOR 0x000000FF

#define HEX_COLOR(hex)                                    \
	((hex) >> (3 * 8)) & 0xFF, ((hex) >> (2 * 8)) & 0xFF, \
		((hex) >> (1 * 8)) & 0xFF, ((hex) >> (0 * 8)) & 0xFF

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

typedef struct {
	float x;
	float y;
} Vec2;

Vec2 vec2(float x, float y) { return (Vec2){x, y}; }

void render_line(SDL_Renderer* renderer, Vec2 begin, Vec2 end, uint32_t color) {
	check_sdl_code(SDL_SetRenderDrawColor(renderer, HEX_COLOR(color)));
	check_sdl_code(
		SDL_RenderDrawLineF(renderer, begin.x, begin.y, end.x, end.y));
}

int main(int argc, char* argv[]) {
	check_sdl_code(SDL_Init(SDL_INIT_VIDEO));
	SDL_Window* const window =
		check_sdl_ptr(SDL_CreateWindow("Bezier Curves", 400, 400, SCREEN_WIDTH,
									   SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE));

	SDL_Renderer* const renderer =
		check_sdl_ptr(SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED));

	int quit = 0;

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
					}
				}
			}
		}

		check_sdl_code(
			SDL_SetRenderDrawColor(renderer, HEX_COLOR(BACKGROUND_COLOR)));
		check_sdl_code(SDL_RenderClear(renderer));

		render_line(renderer, vec2(0.0f, 0.0f),
					vec2(SCREEN_WIDTH, SCREEN_HEIGHT), 0x00FF00FF);

		SDL_RenderPresent(renderer);
		SDL_Log("Rendered...\n");
	}

	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	return 0;
}
