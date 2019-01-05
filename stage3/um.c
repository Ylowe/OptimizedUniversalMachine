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
#include "registers.h"
#include "memory.h"
#include "memory_interface.h"
#include "alu.h"
#include "io.h"

#define BYTE_WIDTH 8

typedef uint32_t UM_instruction;
typedef enum Um_opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, MAP, UNMAP, OUT, IN, LOADP, LV
} Um_opcode;

Memory_T           UM_boot(char *path);
UM_instruction     UM_fetch(Memory_T mem, uint32_t pc);
uint32_t           UM_execute(Memory_T mem, UM_instruction instr, uint32_t pc);

/* global register array */
uint32_t reg[8] = {0};

int main(int argc, char *argv[])
{
        uint32_t pc = 0;

        if(argc != 2){
                fprintf(stderr, "Usage: %s [.um binary file]\n", argv[0]);
                exit(EXIT_FAILURE);
        }

        UM_instruction instr;
        Memory_T mem = UM_boot(argv[1]);


        while (1) {
                instr = UM_fetch(mem, pc);
                pc = UM_execute(mem, instr, pc);

        }

        return 0;
}

Memory_T UM_boot(char *path)
{
        Memory_T mem = Memory_new();
        struct stat pgm_stats;
        FILE *program;
        UM_instruction instr;
        int pgm_bytes, pgm_length, i, lsb, byte;
        
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
        return mem;
}

UM_instruction UM_fetch(Memory_T mem, uint32_t pc)
{
        return *Memory_at(mem, 0, (int)pc);
}

uint32_t UM_execute(Memory_T mem, UM_instruction instr, uint32_t pc)
{
        reg_name ra, rb, rc;
        Um_opcode opcode;
        uint32_t value;
        unsigned ra_lsb, rb_lsb, rc_lsb;
        unsigned op_lsb = 28;
        unsigned reg_w = 3;
        unsigned op_w = 4;
        unsigned val_w = 25;
        pc++;

        // opcode = Bitpack_getu(instr, opcode_width, opcode_lsb);
        opcode = (instr & (~0U >> (32 - op_w) << op_lsb)) >> op_lsb;

        if (opcode == LV) {
                ra_lsb = 25;

                value = (instr & (~0U >> (32 - val_w)));
                ra = (instr & (~0U >> (32 - reg_w) << ra_lsb)) >> ra_lsb;
                
                ALU_loadval(ra, value);

                return pc;
        }

        ra_lsb = 6;
        rb_lsb = 3;
        rc_lsb = 0;

        ra = (instr & (~0U >> (32 - reg_w) << ra_lsb)) >> ra_lsb;
        rb = (instr & (~0U >> (32 - reg_w) << rb_lsb)) >> rb_lsb;
        rc = (instr & (~0U >> (32 - reg_w) << rc_lsb)) >> rc_lsb;

        // ra = Bitpack_getu(instr, reg_w, ra_lsb);
        // rb = Bitpack_getu(instr, reg_w, rb_lsb);
        // rc = Bitpack_getu(instr, reg_w, rc_lsb);

        switch (opcode) {
                case CMOV:
                        ALU_cmov(ra, rb, rc);
                        break;
                case SLOAD:
                        MEM_seg_load(mem, ra, rb, rc);
                        break;
                case SSTORE:
                        MEM_seg_str(mem, ra, rb, rc);
                        break;
                case ADD:
                        ALU_add(ra, rb, rc);
                        break;
                case MUL:
                        ALU_mul(ra, rb, rc);
                        break;
                case DIV:
                        ALU_div(ra, rb, rc);
                        break;
                case NAND:
                        ALU_nand(ra, rb, rc);
                        break;
                case HALT:
                        Memory_free(&mem);
                        exit(EXIT_SUCCESS);
                        break;
                case MAP: 
                        MEM_map_seg(mem, rb, rc);
                        break;
                case UNMAP:
                        MEM_unmap_seg(mem, rc);
                        break;
                case OUT:
                        IO_output(stdout, rc);
                        break;
                case IN:
                        IO_input(stdin, rc);
                        break;
                case LOADP:
                        pc = MEM_load_pgm(mem, rb, rc);
                        break;
                default:
                        exit(EXIT_FAILURE);
        }        

        return pc;
}