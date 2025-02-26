#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

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
					case (SDLK_9): {
						quit = 1;
						exit(0);
					}
				}
			}
		}

		SDL_Color x = {.r = 0, .g = 0, .b = 0, .a = 255};

		check_sdl_code(SDL_SetRenderDrawColor(renderer, x.r, x.g, x.b, x.a));
	}

	SDL_Quit();
	return 0;
}

// float lerp(float a, float b, float p) { return a * p + b * (1 - p); }

// float midpoint(float a, float b) { return (lerp(a, b, 0.5)); }