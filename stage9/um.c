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

#define BYTE_WIDTH 8

typedef uint32_t UM_instruction;
typedef enum Um_opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, MAP, UNMAP, OUT, IN, LOADP, LV
} Um_opcode;

typedef struct Segment
{
        unsigned size;
        uint32_t data[];        
} Segment;

typedef enum reg_name { r0 = 0, r1, r2, r3, r4, r5, r6, r7 } reg_name;

int main(int argc, char *argv[])
{
        uint32_t pc = 0;
        uint32_t reg[8] = {0};
        char *path = argv[1];
        UM_instruction instr;

        Segment** memory;
        int memory_top = 0;
        int memory_length = 32768;
        uint32_t* ids;
        int ids_top = 0;
        int ids_length = 32768;
        Segment *seg0;

        struct stat pgm_stats;
        FILE *program;
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
        int id;

        if(argc != 2){
                fprintf(stderr, "Usage: %s [.um binary file]\n", argv[0]);
                exit(EXIT_FAILURE);
        }

        program = fopen(path, "r");

        if (program == NULL){
                fprintf(stderr, "Could not open .um binary %s\n", path);
                exit(EXIT_FAILURE);
        }
        /* boot */

        memory = malloc(sizeof(Segment*) * 32768);
        ids = malloc(sizeof(uint32_t) * 32768);
        
        stat(path, &pgm_stats);
        pgm_bytes = pgm_stats.st_size;

        pgm_length = pgm_bytes/4;

        seg0 = malloc(sizeof(*seg0) + sizeof(uint32_t) * pgm_length);
        seg0->size = pgm_length;

        memory[memory_top] = seg0;
        memory_top++;

        for (i = 0; i < pgm_length; i++) {
                instr = 0;
                for (lsb = 32 - BYTE_WIDTH; lsb >= 0; lsb -= BYTE_WIDTH) {
                        byte = getc(program);
                        instr = instr + (byte << lsb);
                }
                seg0->data[i] = instr;
        }

        fclose(program);

        while (1) {
                /* fetch */
                instr = seg0->data[pc];

                /* decode */
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

                Segment *seg = NULL;

                /* execute */

                switch (opcode) {
                        case CMOV:
                                if (reg[rc] != 0) {
                                        reg[ra] = reg[rb];
                                }                                
                                break;
                        case SLOAD:
                                seg = memory[reg[rb]];
                                reg[ra] = seg->data[reg[rc]];
                                break;
                        case SSTORE:
                                seg = memory[reg[ra]];
                                seg->data[reg[rb]] = reg[rc];
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
                                for (i = 0; i < memory_top; i++) {
                                        seg = memory[i];
                                        if (seg != NULL)
                                                free(seg);
                                }

                                free(memory);
                                free(ids);
                                exit(EXIT_SUCCESS);
                                break;
                        case MAP: 
                                seg = malloc(sizeof(*seg) + sizeof(uint32_t) * reg[rc]);
                                seg->size = reg[rc];
                                
                                for (i = 0; i < (int)reg[rc]; i++)
                                        seg->data[i] = 0;

                                if (ids_top == 0) {

                                        id = memory_top;
                                        if(id == memory_length){
                                                Segment** mem_new = malloc(sizeof(Segment*) * memory_length*2);
                                                for(i = 0; i < memory_length; i++) {
                                                        mem_new[i] = memory[i];
                                                }
                                                free(memory);

                                                memory = mem_new;
                                                memory_length *= 2;
                                        }
                                        memory[id] = seg;
                                        memory_top++;
                                } else {
                                        id = ids[ids_top - 1];
                                        ids_top--;
                                        memory[id] = seg;
                                }

                                reg[rb] = id;
                                break;
                        case UNMAP:
                                seg = memory[reg[rc]];
                                free(seg);
                                memory[reg[rc]] = NULL;
                                if(ids_top == ids_length){
                                        uint32_t* new_ids = malloc(sizeof(uint32_t) * ids_length*2);
                                        for(i = 0; i < ids_length; i++){
                                                new_ids[i] = ids[i];
                                        }
                                        free(ids);
                                        ids = new_ids;
                                        ids_length *= 2;
                                }
                                ids[ids_top] = reg[rc];
                                ids_top++;
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
                                pc = reg[rc];

                                if (reg[rb] == 0) {
                                        break;
                                }

                                seg = memory[reg[rb]];
                                int prog_len = seg->size;

                                free(seg0);
                                seg0 = malloc(sizeof(*seg0) + sizeof(uint32_t) * prog_len);
                                memory[0] = seg0;

                                for (i = 0; i < prog_len; i++) {
                                        seg0->data[i] = seg->data[i];
                                }

                                break;
                        default:
                                exit(EXIT_FAILURE);
                }        
        }

        return 0;
}
