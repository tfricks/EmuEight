#include "unity.h"
#include "cpu.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

chip8_t *p_cpu;

// Mock functions if needed
void setUp(void) 
{
    // Set up code before each test
    p_cpu = cpu_init();
}

void tearDown(void) 
{
    // Clean up code after each test
    free(p_cpu);
}

void test_00e0(void) 
{
    // 0x00E0 (CLS) Clear the display
    memset(p_cpu->vram, 1, sizeof(p_cpu->vram));
    p_cpu->memory[p_cpu->pc] = 0x00;
    p_cpu->memory[p_cpu->pc + 1] = 0xE0;
    cpu_cycle(p_cpu);
    for(int i = 0; i < DISPLAY_H * DISPLAY_W; i++) {
        TEST_ASSERT_EQUAL(0, p_cpu->vram[i]);
    }
}

void test_00ee(void) 
{
    // 0x00EE (RET) Return from a subroutine
    p_cpu->memory[p_cpu->pc] = 0x00;
    p_cpu->memory[p_cpu->pc + 1] = 0xEE;
    p_cpu->sp = 1;
    p_cpu->stack[0] = 0x200;
    cpu_cycle(p_cpu);
    TEST_ASSERT_EQUAL(0x200, p_cpu->pc);
    TEST_ASSERT_EQUAL(0, p_cpu->sp);
}

void test_1nnn(void) 
{
    // 0x1nnn (JP) Jump to location nnn
    p_cpu->memory[p_cpu->pc] = 0x12;
    p_cpu->memory[p_cpu->pc + 1] = 0x34;
    cpu_cycle(p_cpu);
    TEST_ASSERT_EQUAL(0x234, p_cpu->pc);
}

void test_2nnn(void) 
{
    // 0x2nnn (CALL) Call subroutine at location nnn
    p_cpu->memory[p_cpu->pc] = 0x23;
    p_cpu->memory[p_cpu->pc + 1] = 0x45;
    cpu_cycle(p_cpu);
    TEST_ASSERT_EQUAL(0x345, p_cpu->pc);
    TEST_ASSERT_EQUAL(0x202, p_cpu->stack[0]);
    TEST_ASSERT_EQUAL(1, p_cpu->sp);
}

void test_3xnn(void) 
{
    // 0x3xnn (SE) Skip next instruction if Vx = nn
    p_cpu->memory[p_cpu->pc] = 0x30;
    p_cpu->memory[p_cpu->pc + 1] = 0x12;
    p_cpu->V[0] = 0x12;
    cpu_cycle(p_cpu);
    TEST_ASSERT_EQUAL(0x204, p_cpu->pc);
    // Test when Vx != nn
    p_cpu->memory[p_cpu->pc] = 0x30;
    p_cpu->memory[p_cpu->pc + 1] = 0x12;
    p_cpu->V[0] = 0x13;
    cpu_cycle(p_cpu);
    TEST_ASSERT_EQUAL(0x206, p_cpu->pc);
}

void test_4xnn(void) 
{
    // 0x4xnn (SNE) Skip next instruction if Vx != nn
    p_cpu->memory[p_cpu->pc] = 0x40;
    p_cpu->memory[p_cpu->pc + 1] = 0x12;
    p_cpu->V[0] = 0x13;
    cpu_cycle(p_cpu);
    TEST_ASSERT_EQUAL(0x204, p_cpu->pc);
    // Test when Vx == nn
    p_cpu->memory[p_cpu->pc] = 0x40;
    p_cpu->memory[p_cpu->pc + 1] = 0x12;
    p_cpu->V[0] = 0x12;
    cpu_cycle(p_cpu);
    TEST_ASSERT_EQUAL(0x206, p_cpu->pc);
}

void test_5xy0(void) 
{
    // 0x5xy0 (SE) Skip next instruction if Vx = Vy
    p_cpu->memory[p_cpu->pc] = 0x50;
    p_cpu->memory[p_cpu->pc + 1] = 0x10;
    p_cpu->V[0] = 0x10;
    p_cpu->V[1] = 0x10;
    cpu_cycle(p_cpu);
    TEST_ASSERT_EQUAL(0x204, p_cpu->pc);
    // Test when Vx != Vy
    p_cpu->memory[p_cpu->pc] = 0x50;
    p_cpu->memory[p_cpu->pc + 1] = 0x10;
    p_cpu->V[0] = 0x10;
    p_cpu->V[1] = 0x11;
    cpu_cycle(p_cpu);
    TEST_ASSERT_EQUAL(0x206, p_cpu->pc);
}

void test_6xnn(void) 
{
    // 0x6xnn (LD) Set Vx = nn
    p_cpu->memory[p_cpu->pc] = 0x60;
    p_cpu->memory[p_cpu->pc + 1] = 0x12;
    cpu_cycle(p_cpu);
    TEST_ASSERT_EQUAL(0x12, p_cpu->V[0]);
}

void test_7xnn(void) 
{
    // 0x7xnn (ADD) Set Vx = Vx + nn
    p_cpu->memory[p_cpu->pc] = 0x70;
    p_cpu->memory[p_cpu->pc + 1] = 0x12;
    p_cpu->V[0] = 0x12;
    cpu_cycle(p_cpu);
    TEST_ASSERT_EQUAL(0x24, p_cpu->V[0]);
}

void test_8xy0(void) 
{
    // 0x8xy0 (LD) Set Vx = Vy
    p_cpu->memory[p_cpu->pc] = 0x80;
    p_cpu->memory[p_cpu->pc + 1] = 0x10;
    p_cpu->V[0] = 0x12;
    p_cpu->V[1] = 0x34;
    cpu_cycle(p_cpu);
    TEST_ASSERT_EQUAL(0x34, p_cpu->V[0]);
}

void test_8xy1(void) 
{
    // 0x8xy1 (OR) Set Vx = Vx OR Vy
    p_cpu->memory[p_cpu->pc] = 0x80;
    p_cpu->memory[p_cpu->pc + 1] = 0x11;
    p_cpu->V[0] = 0x12;
    p_cpu->V[1] = 0x34;
    cpu_cycle(p_cpu);
    TEST_ASSERT_EQUAL(0x36, p_cpu->V[0]);
}

void test_8xy2(void) 
{
    // 0x8xy2 (AND) Set Vx = Vx AND Vy
    p_cpu->memory[p_cpu->pc] = 0x80;
    p_cpu->memory[p_cpu->pc + 1] = 0x12;
    p_cpu->V[0] = 0x12;
    p_cpu->V[1] = 0x34;
    cpu_cycle(p_cpu);
    TEST_ASSERT_EQUAL(0x10, p_cpu->V[0]);
}

void test_8xy3(void) 
{
    // 0x8xy3 (XOR) Set Vx = Vx XOR Vy
    p_cpu->memory[p_cpu->pc] = 0x80;
    p_cpu->memory[p_cpu->pc + 1] = 0x13;
    p_cpu->V[0] = 0x12;
    p_cpu->V[1] = 0x34;
    cpu_cycle(p_cpu);
    TEST_ASSERT_EQUAL(0x26, p_cpu->V[0]);
}

void test_8xy4(void) 
{
    // 0x8xy4 (ADD) Set Vx = Vx + Vy. Set VF = carry
    p_cpu->memory[p_cpu->pc] = 0x80;
    p_cpu->memory[p_cpu->pc + 1] = 0x14;
    p_cpu->V[0] = 0x12;
    p_cpu->V[1] = 0x34;
    cpu_cycle(p_cpu);
    TEST_ASSERT_EQUAL(0x46, p_cpu->V[0]);
    TEST_ASSERT_EQUAL(0, p_cpu->V[0xF]);
    // Test when there is a carry
    p_cpu->memory[p_cpu->pc] = 0x80;
    p_cpu->memory[p_cpu->pc + 1] = 0x14;
    p_cpu->V[0] = 0xFF;
    p_cpu->V[1] = 0x01;
    cpu_cycle(p_cpu);
    TEST_ASSERT_EQUAL(0x00, p_cpu->V[0]);
    TEST_ASSERT_EQUAL(1, p_cpu->V[0xF]);
}

void test_8xy5(void) 
{
    // 0x8xy5 (SUB) Set Vx = Vx - Vy. Set VF = NOT borrow
    p_cpu->memory[p_cpu->pc] = 0x80;
    p_cpu->memory[p_cpu->pc + 1] = 0x15;
    p_cpu->V[0] = 0x34;
    p_cpu->V[1] = 0x12;
    cpu_cycle(p_cpu);
    TEST_ASSERT_EQUAL(0x22, p_cpu->V[0]);
    TEST_ASSERT_EQUAL(1, p_cpu->V[0xF]);
    // Test when there is no borrow
    p_cpu->memory[p_cpu->pc] = 0x80;
    p_cpu->memory[p_cpu->pc + 1] = 0x15;
    p_cpu->V[0] = 0x12;
    p_cpu->V[1] = 0x34;
    cpu_cycle(p_cpu);
    TEST_ASSERT_EQUAL(0xDE, p_cpu->V[0]);
    TEST_ASSERT_EQUAL(0, p_cpu->V[0xF]);
}

void test_8xy6(void) 
{
    // 0x8xy6 (SHR) Set Vx = Vy SHR 1
    p_cpu->memory[p_cpu->pc] = 0x80;
    p_cpu->memory[p_cpu->pc + 1] = 0x16;
    p_cpu->V[0] = 0x00;
    p_cpu->V[1] = 0x12;
    cpu_cycle(p_cpu);
    TEST_ASSERT_EQUAL(0x09, p_cpu->V[0]);
    TEST_ASSERT_EQUAL(0, p_cpu->V[0xF]);
    // Test when there is a carry
    p_cpu->memory[p_cpu->pc] = 0x80;
    p_cpu->memory[p_cpu->pc + 1] = 0x16;
    p_cpu->V[0] = 0x00;
    p_cpu->V[1] = 0x11;
    cpu_cycle(p_cpu);
    TEST_ASSERT_EQUAL(0x08, p_cpu->V[0]);
    TEST_ASSERT_EQUAL(1, p_cpu->V[0xF]);
}

void test_8xy7(void) 
{
    // 0x8xy7 (SUBN) Set Vx = Vy - Vx. Set VF = NOT borrow
    p_cpu->memory[p_cpu->pc] = 0x80;
    p_cpu->memory[p_cpu->pc + 1] = 0x17;
    p_cpu->V[0] = 0x12;
    p_cpu->V[1] = 0x34;
    cpu_cycle(p_cpu);
    TEST_ASSERT_EQUAL(0x22, p_cpu->V[0]);
    TEST_ASSERT_EQUAL(1, p_cpu->V[0xF]);
    // Test when there is no borrow
    p_cpu->memory[p_cpu->pc] = 0x80;
    p_cpu->memory[p_cpu->pc + 1] = 0x17;
    p_cpu->V[0] = 0x34;
    p_cpu->V[1] = 0x12;
    cpu_cycle(p_cpu);
    TEST_ASSERT_EQUAL(0xDE, p_cpu->V[0]);
    TEST_ASSERT_EQUAL(0, p_cpu->V[0xF]);
}

void test_8xye(void) 
{
    // 0x8xyE (SHL) Set Vx = Vy SHL 1
    p_cpu->memory[p_cpu->pc] = 0x80;
    p_cpu->memory[p_cpu->pc + 1] = 0x1E;
    p_cpu->V[0] = 0x12;
    p_cpu->V[1] = 0x34;
    cpu_cycle(p_cpu);
    TEST_ASSERT_EQUAL(0x68, p_cpu->V[0]);
    TEST_ASSERT_EQUAL(0, p_cpu->V[0xF]);
    // Test when there is a carry
    p_cpu->memory[p_cpu->pc] = 0x80;
    p_cpu->memory[p_cpu->pc + 1] = 0x1E;
    p_cpu->V[0] = 0x12;
    p_cpu->V[1] = 0x81;
    cpu_cycle(p_cpu);
    TEST_ASSERT_EQUAL(0x02, p_cpu->V[0]);
    TEST_ASSERT_EQUAL(1, p_cpu->V[0xF]);
}

void test_9xy0(void) 
{
    // 0x9xy0 (SNE) Skip next instruction if Vx != Vy
    p_cpu->memory[p_cpu->pc] = 0x90;
    p_cpu->memory[p_cpu->pc + 1] = 0x10;
    p_cpu->V[0] = 0x12;
    p_cpu->V[1] = 0x34;
    cpu_cycle(p_cpu);
    TEST_ASSERT_EQUAL(0x204, p_cpu->pc);
    // Test when Vx == Vy
    p_cpu->memory[p_cpu->pc] = 0x90;
    p_cpu->memory[p_cpu->pc + 1] = 0x10;
    p_cpu->V[0] = 0x12;
    p_cpu->V[1] = 0x12;
    cpu_cycle(p_cpu);
    TEST_ASSERT_EQUAL(0x206, p_cpu->pc);
}

void test_annn(void) 
{
    // 0xAnnn (LD) Set I = nnn
    p_cpu->memory[p_cpu->pc] = 0xA1;
    p_cpu->memory[p_cpu->pc + 1] = 0x23;
    cpu_cycle(p_cpu);
    TEST_ASSERT_EQUAL(0x123, p_cpu->index);
}

void test_bnnn(void) 
{
    // 0xBnnn (JP) Jump to location nnn + V0
    p_cpu->memory[p_cpu->pc] = 0xB1;
    p_cpu->memory[p_cpu->pc + 1] = 0x23;
    p_cpu->V[0] = 0x12;
    cpu_cycle(p_cpu);
    TEST_ASSERT_EQUAL(0x135, p_cpu->pc);
}

void test_cxnn(void) 
{
    // 0xCxnn (RND) Set Vx = random byte AND nn
    p_cpu->memory[p_cpu->pc] = 0xC0;
    p_cpu->memory[p_cpu->pc + 1] = 0x00;
    cpu_cycle(p_cpu);
    TEST_ASSERT_EQUAL(0x00, p_cpu->V[0]);
}

void test_dxyn(void) 
{
    // 0xDxyn (DRW) Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision
    p_cpu->memory[p_cpu->pc] = 0xD0;
    p_cpu->memory[p_cpu->pc + 1] = 0x15;
    p_cpu->V[0] = 0;
    p_cpu->V[1] = 0;
    p_cpu->index = 0;
    p_cpu->memory[0] = 0xFF;
    p_cpu->memory[1] = 0xFF;
    p_cpu->memory[2] = 0xFF;
    p_cpu->memory[3] = 0xFF;
    p_cpu->memory[4] = 0xFF;
    cpu_cycle(p_cpu);
    TEST_ASSERT_EQUAL(0xFFFFFFFF, p_cpu->vram[0]);
    TEST_ASSERT_EQUAL(0xFFFFFFFF, p_cpu->vram[1]);
    TEST_ASSERT_EQUAL(0xFFFFFFFF, p_cpu->vram[2]);
    TEST_ASSERT_EQUAL(0xFFFFFFFF, p_cpu->vram[3]);
    TEST_ASSERT_EQUAL(0xFFFFFFFF, p_cpu->vram[4]);
    TEST_ASSERT_EQUAL(0, p_cpu->V[0xF]);
    // Test when there is a collision
    p_cpu->memory[p_cpu->pc] = 0xD0;
    p_cpu->memory[p_cpu->pc + 1] = 0x15;
    p_cpu->index = 0;
    p_cpu->display_wait = false;
    cpu_cycle(p_cpu);
    TEST_ASSERT_EQUAL(0x0, p_cpu->vram[0]);
    TEST_ASSERT_EQUAL(0x0, p_cpu->vram[1]);
    TEST_ASSERT_EQUAL(0x0, p_cpu->vram[2]);
    TEST_ASSERT_EQUAL(0x0, p_cpu->vram[3]);
    TEST_ASSERT_EQUAL(0x0, p_cpu->vram[4]);
    TEST_ASSERT_EQUAL(1, p_cpu->V[0xF]);
}

void test_ex9e(void) 
{
    // 0xEx9E (SNP) Skip next instruction if key with the value of Vx is pressed
    p_cpu->memory[p_cpu->pc] = 0xE0;
    p_cpu->memory[p_cpu->pc + 1] = 0x9E;
    p_cpu->V[0] = 0;
    p_cpu->keypad_register = 0x01;
    cpu_cycle(p_cpu);
    TEST_ASSERT_EQUAL(0x204, p_cpu->pc);
    // Test when key is not pressed
    p_cpu->memory[p_cpu->pc] = 0xE0;
    p_cpu->memory[p_cpu->pc + 1] = 0x9E;
    p_cpu->V[0] = 0;
    p_cpu->keypad_register = 0x00;
    cpu_cycle(p_cpu);
    TEST_ASSERT_EQUAL(0x206, p_cpu->pc);
}

void test_exa1(void) 
{
    // 0xExA1 (SKNP) Skip next instruction if key with the value of Vx is not pressed
    p_cpu->memory[p_cpu->pc] = 0xE0;
    p_cpu->memory[p_cpu->pc + 1] = 0xA1;
    p_cpu->V[0] = 0;
    p_cpu->keypad_register = 0x00;
    cpu_cycle(p_cpu);
    TEST_ASSERT_EQUAL(0x204, p_cpu->pc);
    // Test when key is pressed
    p_cpu->memory[p_cpu->pc] = 0xE0;
    p_cpu->memory[p_cpu->pc + 1] = 0xA1;
    p_cpu->V[0] = 0;
    p_cpu->keypad_register = 0x01;
    cpu_cycle(p_cpu);
    TEST_ASSERT_EQUAL(0x206, p_cpu->pc);
}

void test_fx07(void) 
{
    // 0xFx07 (LD) Set Vx = delay timer value
    p_cpu->memory[p_cpu->pc] = 0xF0;
    p_cpu->memory[p_cpu->pc + 1] = 0x07;
    p_cpu->delayTimer = 0x12;
    cpu_cycle(p_cpu);
    TEST_ASSERT_EQUAL(0x12, p_cpu->V[0]);
}

void test_fx0a(void) 
{
    // 0xFx0A (LD) Wait for a key press, store the value of the key in Vx
    p_cpu->memory[p_cpu->pc] = 0xF0;
    p_cpu->memory[p_cpu->pc + 1] = 0x0A;
    cpu_cycle(p_cpu);
    TEST_ASSERT_EQUAL(UCHAR_MAX, p_cpu->key_held);
    TEST_ASSERT_EQUAL(p_cpu->pc, 0x200);
    // Press the 2 key 00000100
    p_cpu->keypad_register = 0x04;
    cpu_cycle(p_cpu);
    // Key should be held but we still need to wait for release
    TEST_ASSERT_EQUAL(0x02, p_cpu->key_held);
    TEST_ASSERT_EQUAL(p_cpu->pc, 0x200);
    // Release the key
    p_cpu->keypad_register = 0x00;
    cpu_cycle(p_cpu);
    TEST_ASSERT_EQUAL(0x02, p_cpu->V[0]);
    TEST_ASSERT_EQUAL(UCHAR_MAX, p_cpu->key_held);
    TEST_ASSERT_EQUAL(p_cpu->pc, 0x202);
}

void test_fx15(void) 
{
    // 0xFx15 (LD) Set delay timer = Vx
    p_cpu->memory[p_cpu->pc] = 0xF0;
    p_cpu->memory[p_cpu->pc + 1] = 0x15;
    p_cpu->V[0] = 0x12;
    cpu_cycle(p_cpu);
    TEST_ASSERT_EQUAL(0x12, p_cpu->delayTimer);
}

void test_fx18(void) 
{
    // 0xFx18 (LD) Set sound timer = Vx
    p_cpu->memory[p_cpu->pc] = 0xF0;
    p_cpu->memory[p_cpu->pc + 1] = 0x18;
    p_cpu->V[0] = 0x12;
    cpu_cycle(p_cpu);
    TEST_ASSERT_EQUAL(0x12, p_cpu->soundTimer);
}

void test_fx1e(void) 
{
    // 0xFx1E (ADD) Set I = I + Vx
    p_cpu->memory[p_cpu->pc] = 0xF0;
    p_cpu->memory[p_cpu->pc + 1] = 0x1E;
    p_cpu->index = 0x12;
    p_cpu->V[0] = 0x34;
    cpu_cycle(p_cpu);
    TEST_ASSERT_EQUAL(0x46, p_cpu->index);
}

void test_fx29(void) 
{
    // 0xFx29 (LD) Set I = location of sprite for digit Vx
    p_cpu->memory[p_cpu->pc] = 0xF0;
    p_cpu->memory[p_cpu->pc + 1] = 0x29;
    p_cpu->V[0] = 0x0;
    cpu_cycle(p_cpu);
    TEST_ASSERT_EQUAL(0, p_cpu->index);
}

void test_fx33(void) 
{
    // 0xFx33 (LD) Store BCD representation of Vx in memory locations I, I+1, and I+2
    p_cpu->memory[p_cpu->pc] = 0xF0;
    p_cpu->memory[p_cpu->pc + 1] = 0x33;
    p_cpu->V[0] = 123;
    p_cpu->index = 0;
    cpu_cycle(p_cpu);
    TEST_ASSERT_EQUAL(1, p_cpu->memory[0]);
    TEST_ASSERT_EQUAL(2, p_cpu->memory[1]);
    TEST_ASSERT_EQUAL(3, p_cpu->memory[2]);
}

void test_fx55(void) 
{
    // 0xFx55 (LD) Store registers V0 through Vx in memory starting at location I
    p_cpu->memory[p_cpu->pc] = 0xF4;
    p_cpu->memory[p_cpu->pc + 1] = 0x55;
    p_cpu->V[0] = 0x01;
    p_cpu->V[1] = 0x02;
    p_cpu->V[2] = 0x03;
    p_cpu->V[3] = 0x04;
    p_cpu->V[4] = 0x05;
    p_cpu->index = 0;
    cpu_cycle(p_cpu);
    TEST_ASSERT_EQUAL(0x01, p_cpu->memory[0]);
    TEST_ASSERT_EQUAL(0x02, p_cpu->memory[1]);
    TEST_ASSERT_EQUAL(0x03, p_cpu->memory[2]);
    TEST_ASSERT_EQUAL(0x04, p_cpu->memory[3]);
    TEST_ASSERT_EQUAL(0x05, p_cpu->memory[4]);
    //TEST_ASSERT_EQUAL(0, p_cpu->index);
}

void test_fx65(void) 
{
    // 0xFx65 (LD) Read registers V0 through Vx from memory starting at location I
    p_cpu->memory[p_cpu->pc] = 0xF4;
    p_cpu->memory[p_cpu->pc + 1] = 0x65;
    p_cpu->memory[0] = 0x01;
    p_cpu->memory[1] = 0x02;
    p_cpu->memory[2] = 0x03;
    p_cpu->memory[3] = 0x04;
    p_cpu->memory[4] = 0x05;
    p_cpu->index = 0;
    cpu_cycle(p_cpu);
    TEST_ASSERT_EQUAL(0x01, p_cpu->V[0]);
    TEST_ASSERT_EQUAL(0x02, p_cpu->V[1]);
    TEST_ASSERT_EQUAL(0x03, p_cpu->V[2]);
    TEST_ASSERT_EQUAL(0x04, p_cpu->V[3]);
    TEST_ASSERT_EQUAL(0x05, p_cpu->V[4]);
    //TEST_ASSERT_EQUAL(0, p_cpu->index);
}

int main(void) 
{
    UNITY_BEGIN();
    RUN_TEST(test_00e0);
    RUN_TEST(test_00ee);
    RUN_TEST(test_1nnn);
    RUN_TEST(test_2nnn);
    RUN_TEST(test_3xnn);
    RUN_TEST(test_4xnn);
    RUN_TEST(test_5xy0);
    RUN_TEST(test_6xnn);
    RUN_TEST(test_7xnn);
    RUN_TEST(test_8xy0);
    RUN_TEST(test_8xy1);
    RUN_TEST(test_8xy2);
    RUN_TEST(test_8xy3);
    RUN_TEST(test_8xy4);
    RUN_TEST(test_8xy5);
    RUN_TEST(test_8xy6);
    RUN_TEST(test_8xy7);
    RUN_TEST(test_8xye);
    RUN_TEST(test_9xy0);
    RUN_TEST(test_annn);
    RUN_TEST(test_bnnn);
    RUN_TEST(test_cxnn);
    RUN_TEST(test_dxyn);
    RUN_TEST(test_ex9e);
    RUN_TEST(test_exa1);
    RUN_TEST(test_fx07);
    RUN_TEST(test_fx0a);
    RUN_TEST(test_fx15);
    RUN_TEST(test_fx18);
    RUN_TEST(test_fx1e);
    RUN_TEST(test_fx29);
    RUN_TEST(test_fx33);
    RUN_TEST(test_fx55);
    RUN_TEST(test_fx65);
    return UNITY_END();
}
