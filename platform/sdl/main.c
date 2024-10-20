#include <SDL2/SDL_scancode.h>
#include <stdio.h>
#include <SDL2/SDL.h>

#include "cpu.h"

#define SCREEN_WIDTH CHIP8_DISPLAY_W * 20
#define SCREEN_HEIGHT CHIP8_DISPLAY_H * 20

const static SDL_Scancode KEYMAP[16] = {
    SDL_SCANCODE_X, // 0
    SDL_SCANCODE_1, // 1
    SDL_SCANCODE_2, // 2
    SDL_SCANCODE_3, // 3
    SDL_SCANCODE_Q, // 4
    SDL_SCANCODE_W, // 5
    SDL_SCANCODE_E, // 6
    SDL_SCANCODE_A, // 7
    SDL_SCANCODE_S, // 8
    SDL_SCANCODE_D, // 9
    SDL_SCANCODE_Z, // A
    SDL_SCANCODE_C, // B
    SDL_SCANCODE_4, // C
    SDL_SCANCODE_R, // D
    SDL_SCANCODE_F, // E
    SDL_SCANCODE_V  // F
};

// maybe do better here
int8_t get_index_from_scancode(SDL_Scancode scancode)
{
    // Iterate through the KEYMAP array to find the matching scancode
    for (int8_t i = 0; i < 16; i++)
	{
        if (KEYMAP[i] == scancode)
		{
            return i; // Return the index if found
        }
    }
    return -1; // Return -1 if scancode not found
}

void update_display(SDL_Texture *p_tex, SDL_Renderer *p_ren, void *p_vram, int pitch) 
{
    SDL_UpdateTexture(p_tex, NULL, p_vram, pitch);
    SDL_RenderClear(p_ren);
    SDL_RenderCopy(p_ren, p_tex, NULL, NULL);
    SDL_RenderPresent(p_ren);
}

int main(int argc, char *argv[]){

    (void)argc;

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

    SDL_Texture *tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, CHIP8_DISPLAY_W, CHIP8_DISPLAY_H);

    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);

	int videoPitch = sizeof(uint32_t) * CHIP8_DISPLAY_W;

	chip8_t *p_cpu = cpu_init();

	

	if(!cpu_load_program_from_file(p_cpu, argv[1])) 
	{
		printf("Failed to load program.");
	}

	uint64_t last_cycle_time = SDL_GetTicks64();
	uint64_t last_display_time = SDL_GetTicks64();

    // Keep the main loop until the window is closed (SDL_QUIT event)
    bool exit = false;
    SDL_Event eventData;
    while (!exit)
    {
		uint64_t current_time = SDL_GetTicks64();
		int8_t index = 0;
		if(current_time - last_cycle_time >= 2)
		{
			cpu_cycle(p_cpu);
			cpu_cycle(p_cpu);
			last_cycle_time = SDL_GetTicks();
		}
		if(current_time - last_display_time > 17)
		{
			if(p_cpu->delayTimer > 0)
			{
				p_cpu->delayTimer--;
			}
			if(p_cpu->soundTimer > 0)
			{
				p_cpu->soundTimer--;
			}
			update_display(tex, ren, p_cpu->vram, videoPitch);
			p_cpu->display_wait = false;
			last_display_time = SDL_GetTicks64();
		}
        while (SDL_PollEvent(&eventData))
        {
            switch (eventData.type)
            {
				case SDL_QUIT:
					exit = true;
					break;
				case SDL_KEYDOWN:
            		index = get_index_from_scancode(eventData.key.keysym.scancode);
            		if (index != -1) 
					{
                		p_cpu->keypad_register |= 1 << index;
					}
					break;
				case SDL_KEYUP:
					index = get_index_from_scancode(eventData.key.keysym.scancode);
            		if (index != -1) 
					{
                		p_cpu->keypad_register &= ~(1 << index);
					}
					break;
            }
        }
    }

	free(p_cpu);
	p_cpu = NULL;
    // Free the texture
    SDL_DestroyTexture(tex);

    // Destroy the render, window and finalise SDL
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();

}
