#ifndef CPU_H_   
#define CPU_H_

#include <stdbool.h>
#include <stdint.h>


#define START_ADDRESS 0x200

#define NUM_REGISTERS 16
#define MEMORY_SIZE 4096
#define PROGRAM_MEMORY_SIZE MEMORY_SIZE - START_ADDRESS
#define STACK_SIZE 16
#define DISPLAY_W 64
#define DISPLAY_H 32
#define FONT_SPRITES_SIZE 80
#define FONT_ADDRESS 0
#define FONT_BYTES 5

typedef struct opcode 
{
    uint16_t code;
    uint16_t nnn;
    uint8_t nn;
    uint8_t n;
    uint8_t y;
    uint8_t x;
    uint8_t op;
} opcode_t;


typedef struct chip8
{
    uint8_t V[NUM_REGISTERS]; // V registers
    uint8_t memory[MEMORY_SIZE];
    uint16_t index;
    uint16_t keypad_register;
    uint16_t pc; // Program counter
    uint16_t stack[STACK_SIZE];
    uint8_t sp; // stack pointer
    uint8_t delayTimer;
    uint8_t soundTimer;
    uint32_t vram[DISPLAY_H * DISPLAY_W];
    uint8_t key_held;
    bool display_wait;
} chip8_t;

chip8_t *cpu_init(void);
bool cpu_reset(chip8_t *p_cpu);
bool cpu_load_program(chip8_t *p_cpu, char *p_filename);
void cpu_cycle(chip8_t *p_cpu);

#endif // CPU_H_
