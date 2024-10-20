#ifndef CPU_H_   
#define CPU_H_

#include <_types/_uint8_t.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>


static const uint16_t CHIP8_NUM_REGISTERS = 16;
static const uint16_t CHIP8_START_ADDRESS = 0x200;
static const uint16_t CHIP8_MEMORY_SIZE = 4096;
static const uint16_t CHIP8_PROGRAM_MEMORY_SIZE = CHIP8_MEMORY_SIZE - CHIP8_START_ADDRESS;
static const uint16_t CHIP8_STACK_SIZE = 16;
static const uint16_t CHIP8_DISPLAY_W = 64;
static const uint16_t CHIP8_DISPLAY_H = 32;
static const uint16_t CHIP8_FONT_ADDRESS = 0;
static const uint16_t CHIP8_FONT_BYTES = 5;

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
    uint8_t V[CHIP8_NUM_REGISTERS]; // V registers
    uint8_t memory[CHIP8_MEMORY_SIZE];
    uint16_t index;
    uint16_t keypad_register;
    uint16_t pc; // Program counter
    uint16_t stack[CHIP8_STACK_SIZE];
    uint8_t sp; // stack pointer
    uint8_t delayTimer;
    uint8_t soundTimer;
    uint32_t vram[CHIP8_DISPLAY_H * CHIP8_DISPLAY_W];
    uint8_t key_held;
    bool display_wait;
    uint8_t ipf; // instructions per frame
} chip8_t;

chip8_t *cpu_init(void);
bool cpu_reset(chip8_t *p_cpu);
bool cpu_load_program_from_file(chip8_t *p_cpu, char *p_filename);
bool cpu_load_program_from_buffer(chip8_t *p_cpu, const uint8_t *p_buffer, size_t buffer_size);
void cpu_cycle(chip8_t *p_cpu);

#endif // CPU_H_
