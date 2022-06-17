// CPE 517
// Lab 2
//"I pledge my honor that I have abided by the Stevens Honor System."
#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shell.h"

uint8_t op;
uint32_t instruction;
uint32_t addr_rs, addr_rt, addr_rd;
uint32_t rs, rt, rd;  // to save the register value
uint8_t func;
uint16_t itemp;  // for save immidiate number
                 // please take a loot at https:  //
                 // inst.eecs.berkeley.edu/~cs61c/resources/MIPS_Green_Sheet.pdf
                 //  or
//  https://web.cse.ohio-state.edu/~crawfis.3/cse675-02/Slides/MIPS%20Instruction%20Set.pdf

void fetch() {
  printf("fetch:\n");
  instruction = mem_read_32(CURRENT_STATE.PC);  // the instruction
  NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
// instruction = 2009000a addi $t1, $zero, 10
// 0010 0000 0000 1001 0000 0000 0000 1010
// 001000 00000 01001 0000 0000 0000 0000 1010
// opcode rs rt rd immediate
void decode() {
  printf("decode: ");
  op = instruction >> 26;  // find the 31-26 bit
  // 001000 00000 01001 0000 0000 0000 1010
  // >> 26
  // 0000 0000 0000 0000 0000 0000 001000 = 8
  // printf("the op: %x ",op);
  // if op==0, it is R type
  if (op == 0) {
    // 012a5822
    //  0000 0001 0010 1010 0101 1000 0010 0010
    //  000000 01001 01010 01011 00000 100010
    //  op rs rt rd shamt funct
    printf("R instruction\n");
    addr_rs = (instruction >> 21) & 0x1f;
    addr_rt = (instruction >> 16) & 0x1f;
    addr_rd = (instruction >> 11) & 0x1f;
    rs = CURRENT_STATE.REGS[addr_rs];  // 2^5=32
    rt = CURRENT_STATE.REGS[addr_rt];
    // rd=CURRENT_STATE.REGS[addr_rd];
    func = instruction & 0x3f;
    // 0000 0001 0010 0000 0010 0000 0010 0000
    // 000000 01001 00000 00100 00000 100000 = add
    // op rs rt rd shamt funct
    //
    // 0000 0001 0010 0000 0010 0000 0010 0000
    // &
    // 0000 0000 0000 0000 0000 0000 0011 1111 =
    // 00100000 = 32
    // 012a5822 -> sub instruction
    // 0000 0001 0010 1010 0101 1000 0010 0010
    // &
    // 0000 0000 0000 0000 0000 0000 0011 1111
    // 100010 = 34
    // 012a5821 -> addu instruction
    // 0000 0001 0010 1010 0101 1000 0010 0001
    // 000000 01001 01010 01011 00000 100001 -> addu function = 33
    // op rs rt rd shamt funct
  }
  if (op == 8) {  // addi:001000
    printf("addi instruction \n");
    addr_rs = (instruction >> 21) & 0x1f;
    addr_rt = (instruction >> 16) & 0x1f;
    rs = CURRENT_STATE.REGS[addr_rs];  // 2^5=32
    itemp = instruction & 0x0000ffff;
    // 001000 00000 01001 0000 0000 0000 1010
    // op rs rt immediate
    // 0000 0000 0000 0000 00000 001000 00000
    // op rs
    // &
    // 000 11111 -> 0x1f extracts only the last 5 digits
    //
  }
  if (op == 9) {  // addiu:001001
    // 2409000c
    // 0010 0100 0000 1001 0000 0000 0000 1100
    // 001001 00000 01001 0000 0000 0000 1100
    // op rs rt immediate
    printf("addiu instruction \n");
    addr_rs = (instruction >> 21) & 0x1f;
    addr_rt = (instruction >> 16) & 0x1f;
    rs = CURRENT_STATE.REGS[addr_rs];  // 2^5=32
    itemp = instruction & 0x0000ffff;
    rt = rs + itemp;
    if (itemp > 32767) {   // 2^15-1=32767 is the largest number
      itemp = ~itemp + 1;  // the ~ flips the bits
      rt = rs - itemp;
    } else {
      rt = rs + itemp;
    }
    NEXT_STATE.REGS[addr_rt] =
        rt;  // It is fine if you just execute here instead of executing in
             // execute() function.
  }
}
void execute() {
  printf("execute: ");
  if (op == 0) {
    switch (func) {
      case 36:  // and:100100 = 36
        printf("and instruction\n");
        rd = 0;
        rd = rs & rt;
        NEXT_STATE.REGS[addr_rd] = rd;
        break;
      case 35:  // sub:100011 = 35
        printf("subu instruction\n");
        rd = 0;
        rd = rs - rt;
        NEXT_STATE.REGS[addr_rd] = rd;
        break;
      case 34:  // sub:100010 = 34
        printf("sub instruction\n");
        rd = 0;
        rd = rs - rt;
        NEXT_STATE.REGS[addr_rd] = rd;
        break;
      case 33:  // addu:100001 = 33
        printf("addu instruction\n");
        rd = 0;
        rd = rs + rt;
        NEXT_STATE.REGS[addr_rd] = rd;
        break;
      case 32:  // add:100000 = 32
        printf("add instruction \n");
        rd = 0;
        // if(rt>2147483647)
        rd = rt + rs;
        NEXT_STATE.REGS[addr_rd] = rd;
        break;
      case 31:
        printf("jump instruction\n");
        uint32_t Jadd = ((instruction & 0x3ffffff) << 2);
        Jadd = Jadd || (mem_read_32(CURRENT_STATE.PC) & (0xf << 28));
        NEXT_STATE.PC = Jadd;
        break;
      case 12:                              // system call:001100 = 12
        if (CURRENT_STATE.REGS[2] == 10) {  // v0==10 then exit
          printf("systemcall: exit\n");
          RUN_BIT = FALSE;
        }
        if (CURRENT_STATE.REGS[2] == 1) {  // v0==1: print int
          rt = CURRENT_STATE.REGS[4];      // a0 is 4th register
          printf("print the number: %d \n", rt);
        }
        break;
      default:
        break;
    }
  }
  if (op == 8) {
    printf("addi instruction \n");
    if (itemp > 32767) {  // 2^15-1=32767 is the largest number
      itemp = ~itemp + 1;
      rt = rs - itemp;
    } else {
      rt = rs + itemp;
    }
    NEXT_STATE.REGS[addr_rt] =
        rt;  // It is fine if you just execute here instead of executing in
             // execute() function.
  }
  if(op == 2){
    printf("jump instruction\n");
        uint32_t Jadd = ((instruction & 0x3ffffff) << 2);
        Jadd = Jadd || (mem_read_32(CURRENT_STATE.PC) & (0xf << 28));
        NEXT_STATE.PC = Jadd;
  }
  if(op == 3){
      printf("jal execution \n"); //extra credit
      uint32_t addr_jal = ((instruction & 0x3ffffff) << 2);
      addr_jal += CURRENT_STATE.PC & (0xf << 28);
      NEXT_STATE.PC = addr_jal;
      NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4; //31 -- location of return address
  }
}
void process_instruction() {
  /* execute one instruction here. You should use CURRENT_STATE and modify
   * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
   * access memory. */
  fetch();
  decode();
  execute();
  printf("Finished 1 instruction\n\n");
}
