
#define _NO_CRT_STDIO_INLINE
int _fltused;

// kernel32.lib;user32.lib;gdi32.lib;winspool.lib;comdlg32.lib;advapi32.lib;shell32.lib;ole32.lib;oleaut32.lib;uuid.lib;odbc32.lib;odbccp32.lib;%(AdditionalDependencies)

// kernel32.lib; w:/msvcrt_test/msvcrt.lib; gdi32.lib; user32.lib; opengl32.lib; %(AdditionalDependencies)

// kernel32.lib;user32.lib;gdi32.lib;winspool.lib;comdlg32.lib;advapi32.lib;shell32.lib;ole32.lib;oleaut32.lib;uuid.lib;odbc32.lib;odbccp32.lib;%(AdditionalDependencies) w:/msvcrt_test/msvcrt.lib; opengl32.lib
// %(AdditionalDependencies);   w:/msvcrt_test/msvcrt32.lib; kernel32.lib; user32.lib; gdi32.lib; opengl32.lib;       winspool.lib;comdlg32.lib;advapi32.lib;shell32.lib;ole32.lib;oleaut32.lib;uuid.lib;odbc32.lib;odbccp32.lib;      

//include <stdafx.h>

//#pragma comment(lib, "kernel32")
//#pragma comment(lib, "w:\msvcrt_test\msvcrt32.lib")
//#pragma comment(lib, "user32.lib")
//#pragma comment(lib, "gdi32.lib")
//#pragma comment(lib, "opengl32.lib")
//#pragma comment(lib, "%(AdditionalDependencies)")

#ifdef _WIN32
#	include "w:/libs/jam.c"
#elif __APPLE__
#	include "../../libs/jam.c"
#endif

//#ifdef __APPLE__
//#	define SDL_MAIN_HANDLED
//#	include <SDL2/SDL.h>
//#	include <OpenGL/gl.h>
//#endif

#include <stdio.h>
#include <stdbool.h>

bool quit = false;
int window_width = 1280;
int window_height = 720;


#include "game.c"

#if 0
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
#endif