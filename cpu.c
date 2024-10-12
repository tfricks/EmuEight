#include <limits.h>
#include <time.h>
#include "cpu.h"

static uint8_t font_sprites[FONT_SPRITES_SIZE] =
{
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

CHIP8 *cpu_init(void) 
{
    CHIP8 *p_cpu = NULL;

    p_cpu = malloc(sizeof(*p_cpu));

    if(NULL == p_cpu) 
    {
        // couldn't allocate memory
        return NULL;
    }
    
    // clear the memory
    memset(p_cpu, 0, sizeof(*p_cpu));

    // load font sprites into memory at address 0x00
    memcpy(p_cpu->memory, &font_sprites, FONT_SPRITES_SIZE);
    
    // set the program counter
    p_cpu->pc = START_ADDRESS;

    srand(time(NULL));

    // Caller is responsible for freeing.
    return p_cpu;
}

bool cpu_load_program(CHIP8 *p_cpu, char *p_filename)
{
    FILE *pg_fp;
    size_t pg_size = 0;
    size_t ret_size = 0;

    if(NULL == p_filename) 
    {
        return false;
    }

    pg_fp = fopen(p_filename, "rb");

    if(NULL == pg_fp) 
    {
        // failed to open file
        return false;
    }

    // Get file size
    fseek(pg_fp, 0, SEEK_END);
    ret_size = ftell(pg_fp);
    fseek(pg_fp, 0, SEEK_SET);

    if(ret_size >= PROGRAM_MEMORY_SIZE) 
    {
        return false;
    }

    // Read file into memory
    pg_size = fread(p_cpu->memory + START_ADDRESS, sizeof(p_cpu->memory[0]), ret_size, pg_fp);
    if(pg_size != ret_size) 
    {
        printf("read error");
        // Some manner of file read error
        return false;
    }

    fclose(pg_fp);

    return true;
}

void cpu_cycle(CHIP8 *p_cpu)
{
    OPCODE opcode;
    uint8_t x = 0;
    uint8_t y = 0;
    uint8_t carry = false;
    if(p_cpu->delayTimer > 0)
        p_cpu->delayTimer--;

    // Fetch 
    opcode.code = p_cpu->memory[p_cpu->pc] << 8 | p_cpu->memory[p_cpu->pc + 1];
    p_cpu->pc+=2;

    // Decode and execute
    switch (opcode.op)
    {
        case 0x0:
            switch (opcode.kk)
            {
                case 0xE0: // 0x00E0 (CLS) Clear the display
                    memset(p_cpu->vram, 0, sizeof(p_cpu->vram));
                    break;
                case 0xEE: // 0x00EE (RET) Return from a subroutine.
                    p_cpu->sp--;                    
                    p_cpu->pc = p_cpu->stack[p_cpu->sp];
                    break;
                default:
                    break;
            }
            break;
        case 0x1: // 0x1nnn (JP) Jump to location nnn.
            p_cpu->pc = opcode.nnn;
            break;
        case 0x2: // 0x2nnn (CALL) Call subroutine at location nnn.
            p_cpu->stack[p_cpu->sp] = p_cpu->pc;
            p_cpu->sp++;
            p_cpu->pc = opcode.nnn;
            break;
        case 0x3: // 0x3xkk (SE) Skip next instruction if Vx = kk;
            if(opcode.kk == p_cpu->V[opcode.x])
            {
                p_cpu->pc += 2;
            }
            break;
        case 0x4: // 0x4xkk (SNE) Skip next instruction if Vx != kk;
            if(opcode.kk != p_cpu->V[opcode.x])
            {
                p_cpu->pc += 2;
            }
            break;
        case 0x5: // 0x5xy0 (SE) Skip next instruction if Vx = Vy;
            if(p_cpu->V[opcode.x] == p_cpu->V[opcode.y])
            {
                p_cpu->pc += 2;
            }
            break;
        case 0x6: // 0x6xkk (LD) Set Vx = kk.
            p_cpu->V[opcode.x] = opcode.kk;
            break;
        case 0x7: // 0x7xkk (ADD) Set Vx = Vx + kk.
            p_cpu->V[opcode.x] += opcode.kk;
            break;
        case 0x8:
            switch (opcode.n)
            {
                case 0x0: // 0x8xy0 (LD) Set Vx = Vy
                    p_cpu->V[opcode.x] = p_cpu->V[opcode.y];
                    break;
                case 0x1: // 0x8xy1 (OR) Set Vx = Vx OR Vy.
                    p_cpu->V[opcode.x] = p_cpu->V[opcode.x] | p_cpu->V[opcode.y];
                    break;
                case 0x2: // 0x8xy2 (AND) Set Vx = Vx AND Vy.
                    p_cpu->V[opcode.x] = p_cpu->V[opcode.x] & p_cpu->V[opcode.y];
                    break;
                case 0x3: // 0x8xy3 (XOR) Set Vx = Vx XOR Vy.
                    p_cpu->V[opcode.x] = p_cpu->V[opcode.x] ^ p_cpu->V[opcode.y];
                    break;
                case 0x4: // 0x8xy4 (ADD) Set Vx = Vx + Vy. Set VF = carry.
                    carry = (p_cpu->V[opcode.y] > UCHAR_MAX - p_cpu->V[opcode.x]);
                    p_cpu->V[opcode.x] += p_cpu->V[opcode.y];
                    p_cpu->V[0xF] = carry;
                    break;
                case 0x5: // 0x8xy5 (SUB) Set Vx = Vx - Vy. Set VF = NOT borrow.
                    carry = (p_cpu->V[opcode.x] >= p_cpu->V[opcode.y]);
                    p_cpu->V[opcode.x] -= p_cpu->V[opcode.y];
                    p_cpu->V[0xF] = carry;
                    break;
                case 0x6: // 0x8xy6 (SHR) Set Vx = Vx SHR 1.
                    // TODO: Make quirk configurable
                    carry = p_cpu->V[opcode.x] & 0x1;
                    p_cpu->V[opcode.x] >>= 1;
                    p_cpu->V[0xF] = carry;
                    break;
                case 0x7: // 0x8xy7 (SUBN) Set Vx = Vy - Vx, set VF = NOT borrow.
                    carry = (p_cpu->V[opcode.y] >= p_cpu->V[opcode.x]);
                    p_cpu->V[opcode.x] = p_cpu->V[opcode.y] - p_cpu->V[opcode.x];
                    p_cpu->V[0xF] = carry;
                    break;
                case 0xE: // 0x8xyE (SHL) Set Vx = Vx SHL 1.
                    // TODO: Make quirk configurable
                    carry = (p_cpu->V[opcode.x]) >> 7;
                    p_cpu->V[opcode.x] <<= 1;
                    p_cpu->V[0xF] = carry;
                    break;
                default:
                    break;
            }
            break;
        case 0x9: // 0x9xy0 (SNE) Skip next instruction if Vx != Vy.
            if(p_cpu->V[opcode.x] != p_cpu->V[opcode.y]) 
            {
                p_cpu->pc += 2;
            }
            break;
        case 0xA: // 0xAnnn (LD I) The value of index register I is set to nnn.
            p_cpu->index = opcode.nnn;
            break;
        case 0xB: // 0xBnnn (JP) Jump to location nnn + V0.
            p_cpu->pc = opcode.nnn + p_cpu->V[0x0];
            break;
        case 0xC: // Set Vx = random byte AND kk
            p_cpu->V[opcode.x] = ((uint8_t)(rand() % 255)) & opcode.kk;
            break;
        case 0xD: // 0xDxyn (DRW) Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
            // wrap around screen
            x = p_cpu->V[opcode.x] & (DISPLAY_W - 1);
            y = p_cpu->V[opcode.y] & (DISPLAY_H - 1);
            p_cpu->V[0xF] = 0;
            for(uint8_t i = 0; i < opcode.n; i++)
            {
                uint8_t sprite_byte = p_cpu->memory[p_cpu->index + i];
                for(uint8_t j = 0; j < 8; j++)
                {
                    if(sprite_byte & (0x80u >> j))
                    {
                        uint32_t *p_pixel = &p_cpu->vram[(((y + i) * DISPLAY_W) + (x + j))];
                        if(*p_pixel) 
                        {
                            // Set collision flag
                            p_cpu->V[0xF] = 1;
                        }
                        *p_pixel ^= 0xFFFFFFFF;
                    }
                }
            }
            break;
        case 0xF:
            switch(opcode.kk) 
            {
                case 0x07: // 0xFx07 (LD) Set Vx = delay timer value.
                    p_cpu->V[opcode.x] = p_cpu->delayTimer;
                    break;
                case 0x0A: // 0xFx0A (LD) Wait for a key press, store the value of the key in Vx.
                    // TODO: Implement
                    break;
                case 0x15: // 0xFx15 (LD) Set delay timer = Vx.
                    p_cpu->delayTimer = p_cpu->V[opcode.x];
                    break;
                case 0x18: // 0xFx18 (LD) Set sound timer = Vx.
                    p_cpu->soundTimer = p_cpu->V[opcode.x];
                    break;
                case 0x1E: // 0xFx1E (ADD) Set I = I + Vx.
                    p_cpu->index += p_cpu->V[opcode.x];
                    break;
                case 0x29: // 0xFx29 (LD) Set I = location of sprite for digit Vx.
                    p_cpu->index = (FONT_ADDRESS + (FONT_BYTES * p_cpu->V[opcode.x]));
                    break;
                case 0x33: // 0xFx33 (LD) Store BCD representation of Vx in memory locations I, I+1, and I+2.
                    p_cpu->memory[p_cpu->index] = (uint8_t)(p_cpu->V[opcode.x]/100) % 10;
                    p_cpu->memory[p_cpu->index + 1] = (uint8_t)(p_cpu->V[opcode.x]/10) % 10;
                    p_cpu->memory[p_cpu->index + 2] = (p_cpu->V[opcode.x]) % 10;
                    break;
                case 0x55: // 0xFx55 (LD) Store registers V0 through Vx in memory starting at location I.
                    for(uint8_t i = 0; i <= opcode.x; i++)
                    {
                        p_cpu->memory[p_cpu->index + i] = p_cpu->V[i];
                    }
                    break;
                case 0x65: // 0xFx65 (LD) Read registers V0 through Vx from memory starting at location I.
                    for(uint8_t i = 0; i <= opcode.x; i++)
                    {
                        p_cpu->V[i] = p_cpu->memory[p_cpu->index + i];
                    }
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
}