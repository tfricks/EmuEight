#include <stdio.h>
#include <SDL.h>

#include "cpu.h"

#define SCREEN_WIDTH DISPLAY_W * 10
#define SCREEN_HEIGHT DISPLAY_H * 10

void update_display(SDL_Texture *p_tex, SDL_Renderer *p_ren, void *p_vram, int pitch) 
{
    SDL_UpdateTexture(p_tex, NULL, p_vram, pitch);
    SDL_RenderClear(p_ren);
    SDL_RenderCopy(p_ren, p_tex, NULL, NULL);
    SDL_RenderPresent(p_ren);
}

int main(int argc, char *argv[]){

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_Window *win = SDL_CreateWindow("EmuEight", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (win == NULL) {
        fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    if (ren == NULL) {
        fprintf(stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(win);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    SDL_Texture *tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, DISPLAY_W, DISPLAY_H);

    //SDL_RenderSetLogicalSize(ren, 64, 32);
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);

	int videoPitch = sizeof(uint32_t) * DISPLAY_W;

	CHIP8 *p_cpu = cpu_init();

	

	if(!cpu_load_program(p_cpu, argv[1])) 
	{
		printf("Failed to load program.");
	}

    // Keep the main loop until the window is closed (SDL_QUIT event)
    bool exit = false;
    SDL_Event eventData;
    while (!exit)
    {
		cpu_cycle(p_cpu);
		update_display(tex, ren, p_cpu->vram, videoPitch);
        while (SDL_PollEvent(&eventData))
        {
            switch (eventData.type)
            {
            case SDL_QUIT:
                exit = true;
                break;
            }
        }
    }

	//for(int i = 0; i < SCREEN_HEIGHT; i++) {
//		for(int j = 0; j < SCREEN_WIDTH; j++) {
//			printf("%d ", ((p_cpu->vram[i+j] & 0xFFFFFFFF) && 1));
//		}
//		printf("\n");
//	}

	free(p_cpu);
	p_cpu = NULL;
    // Free the texture
    SDL_DestroyTexture(tex);

    // Destroy the render, window and finalise SDL
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();

}
