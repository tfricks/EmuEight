#ifndef CPU_H_   
#define CPU_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

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

typedef union OPCODE_U 
{
    uint16_t code;
    struct
    {
        uint16_t nnn : 12;
    };
    struct
    {
        uint8_t kk : 8;
    };
    struct 
    {
        uint8_t n : 4;
        uint8_t y : 4;
        uint8_t x : 4;
        uint8_t op : 4;
    };
} OPCODE;


typedef struct CHIP8_S
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
} CHIP8;

CHIP8 *cpu_init(void);
bool cpu_load_program(CHIP8 *p_cpu, char *p_filename);
void cpu_cycle(CHIP8 *p_cpu);

#endif // CPU_H_