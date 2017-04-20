
#include <stdio.h>
#include <stdbool.h>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include <OpenGL/gl.h>

bool quit = false;
int window_width = 1280;
int window_height = 720;

#include "../../libs/math.c"
#include "game.c"

int main() {
	printf("HELLO WORLD\n");
	
	SDL_SetMainReady();
	SDL_Init(SDL_INIT_VIDEO);
	
	SDL_Window *window = SDL_CreateWindow("The Street Life",
										  SDL_WINDOWPOS_CENTERED,
										  SDL_WINDOWPOS_CENTERED,
										  window_width, window_height,
										  SDL_WINDOW_SHOWN|SDL_WINDOW_OPENGL);
	
	/*SDL_GLContext gl_context =*/ SDL_GL_CreateContext(window);
	
	init();
	
	while (!quit) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					quit = true;
					break;
			}
		}
		
		int numkeys;
		const unsigned char *keys = SDL_GetKeyboardState(&numkeys);
		
		input.gas = keys[SDL_SCANCODE_UP];
		input.reverse = keys[SDL_SCANCODE_DOWN];
		input.steering = -keys[SDL_SCANCODE_LEFT] + keys[SDL_SCANCODE_RIGHT];
		
		update();
		
		SDL_GL_SwapWindow(window);
	}
	
	SDL_Quit();
}