#include <stdio.h>
#include "shell.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>


uint8_t op;
uint32_t instruction;
uint32_t addr_rs,addr_rt,addr_rd;
uint32_t rs,rt,rd;      //to save the register value
uint8_t func;
uint16_t itemp;         //for save immidiate number

  //please take a loot at https://inst.eecs.berkeley.edu/~cs61c/resources/MIPS_Green_Sheet.pdf
  //				 or
  //  https://web.cse.ohio-state.edu/~crawfis.3/cse675-02/Slides/MIPS%20Instruction%20Set.pdf
  
void fetch()
{
    printf("fetch ");
    instruction = mem_read_32(CURRENT_STATE.PC); //the instruction 
    NEXT_STATE.PC=CURRENT_STATE.PC+4;
    
}

void decode()
{
//printf("decode ");
    op=instruction>>26; //find the 31-26 bit
    
    
  //  printf("the op: %x ",op);
    // if op==0, it is R type
    if(op==0){

        addr_rs=(instruction>>21) & 0x1f;
        addr_rt=(instruction>>16) & 0x1f;
        addr_rd=(instruction>>11) & 0x1f;
        rs=CURRENT_STATE.REGS[addr_rs];   //2^5=32
        rt=CURRENT_STATE.REGS[addr_rt];
      //  rd=CURRENT_STATE.REGS[addr_rd];
        func= instruction & 0x3f;
        
    }


    if(op==8){          //addi:001000       
        printf("addi instruction \n");
        addr_rs=(instruction>>21) & 0x1f;
        addr_rt=(instruction>>16) & 0x1f;
        rs=CURRENT_STATE.REGS[addr_rs];   //2^5=32
        itemp=instruction & 0x0000ffff;

        if(itemp > 32767){    //2^15-1=32767 is the largest number
            itemp=~itemp+1;
            rt=rs-itemp;    
        }
        else{
            rt=rs+itemp;    
        }
        NEXT_STATE.REGS[addr_rt]=rt; // It is fine if you just execute here instead of executing in execute() function. 
    }
}

void execute()
{
printf("execute");
if(op==0){
    switch (func) {
        case 32:      //add:100000
            printf("add instruction \n");
            rd=0;
      //      if(rt>2147483647)
            rd=rt+rs;
            NEXT_STATE.REGS[addr_rd]=rd;
            break;
    
        case 12:   //system call:001100
            if(CURRENT_STATE.REGS[2]==10){  //v0==10 then exit
                printf("systemcall: exit\n");
                RUN_BIT=FALSE;
            }
            if(CURRENT_STATE.REGS[2]==1){   //v0==1: print int
                rt=CURRENT_STATE.REGS[4];   //a0 is 4th register
                printf("\n print the number:   %d \n ",rt);
                
            }
            break;
        default:
            break;
    }

}
    
    
    
}
void process_instruction()
{
    /* execute one instruction here. You should use CURRENT_STATE and modify
     * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
     * access memory. */
    fetch();
    decode();
    execute();

}
