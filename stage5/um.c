/*
 * Yang Lowe and Benny Roover, 11/20/18
 * Comp 40 HW 6
 * um.c
 *
 * description
*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "memory.h"

#define BYTE_WIDTH 8

typedef uint32_t UM_instruction;
typedef enum Um_opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, MAP, UNMAP, OUT, IN, LOADP, LV
} Um_opcode;

typedef enum reg_name { r0 = 0, r1, r2, r3, r4, r5, r6, r7 } reg_name;

int main(int argc, char *argv[])
{
        uint32_t pc = 0;
        uint32_t reg[8] = {0};
        char *path = argv[1];
        Memory_T mem = Memory_new();
        struct stat pgm_stats;
        FILE *program;
        UM_instruction instr;
        int pgm_bytes, pgm_length, i, lsb, byte;

        reg_name ra, rb, rc;
        Um_opcode opcode;
        uint32_t value;
        unsigned ra_lsb, rb_lsb, rc_lsb;
        unsigned op_lsb = 28;
        unsigned reg_w = 3;
        unsigned op_w = 4;
        unsigned val_w = 25;

        char input;

        if(argc != 2){
                fprintf(stderr, "Usage: %s [.um binary file]\n", argv[0]);
                exit(EXIT_FAILURE);
        }

      
        /* boot function */
        
        stat(path, &pgm_stats);
        pgm_bytes = pgm_stats.st_size;

        pgm_length = pgm_bytes/4;

        Memory_add_segment(mem, pgm_length);

        program = fopen(path, "r");

        if (program == NULL){
                fprintf(stderr, "Could not open .um binary %s\n", path);
                exit(EXIT_FAILURE);
        }

        for (i = 0; i < pgm_length; i++) {
                instr = 0;
                for (lsb = 32 - BYTE_WIDTH; lsb >= 0; lsb -= BYTE_WIDTH) {
                        byte = getc(program);
                        instr = instr + (byte << lsb);
                }

                *Memory_at(mem, 0, i) = instr;
        }

        fclose(program);

        while (1) {
                /* fetch */
                instr = *Memory_at(mem, 0, (int)pc);

                /* execute */
                pc++;

                opcode = (instr & (~0U >> (32 - op_w) << op_lsb)) >> op_lsb;

                if (opcode == LV) {
                        ra_lsb = 25;

                        value = (instr & (~0U >> (32 - val_w)));
                        ra = (instr & (~0U >> (32-reg_w) << ra_lsb)) >> ra_lsb;
                        
                        reg[ra] = value;

                        continue;
                }

                ra_lsb = 6;
                rb_lsb = 3;
                rc_lsb = 0;

                ra = (instr & (~0U >> (32 - reg_w) << ra_lsb)) >> ra_lsb;
                rb = (instr & (~0U >> (32 - reg_w) << rb_lsb)) >> rb_lsb;
                rc = (instr & (~0U >> (32 - reg_w) << rc_lsb)) >> rc_lsb;

                switch (opcode) {
                        case CMOV:
                                if (reg[rc] != 0) {
                                        reg[ra] = reg[rb];
                                }                                
                                break;
                        case SLOAD:
                                reg[ra] = *Memory_at(mem, reg[rb], reg[rc]);
                                break;
                        case SSTORE:
                                *Memory_at(mem, reg[ra], reg[rb]) = reg[rc];
                                break;
                        case ADD:
                                reg[ra] = (reg[rb] + reg[rc]) & ~0UL;
                                break;
                        case MUL:
                                reg[ra] = (reg[rb] * reg[rc]) & ~0UL;
                                break;
                        case DIV:
                                reg[ra] = (reg[rb] / reg[rc]);
                                break;
                        case NAND:
                                reg[ra] = ~(reg[rb] & reg[rc]);
                                break;
                        case HALT:
                                Memory_free(&mem);
                                exit(EXIT_SUCCESS);
                                break;
                        case MAP: 
                                reg[rb] = Memory_add_segment(mem, reg[rc]);
                                break;
                        case UNMAP:
                                Memory_remove_segment(mem, (int)reg[rc]);
                                break;
                        case OUT:
                                putc((uint8_t)reg[rc], stdout);
                                break;
                        case IN:
                                input = getc(stdin);
                                if (input == EOF) {
                                        reg[rc] = ~0;
                                }
                                else {
                                        reg[rc] = input;
                                }
                                break;
                        case LOADP:
                                if (reg[rb] != 0) {
                                        Memory_load_program(mem, reg[rb]);           
                                }
                                pc = reg[rc];
                                break;
                        default:
                                exit(EXIT_FAILURE);
                }        
        }

        return 0;
}