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
#include <stack.h>

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

// Segment** expand(Segment** memory, int length);
int main(int argc, char *argv[])
{
        uint32_t pc = 0;
        uint32_t reg[8] = {0};
        char *path = argv[1];

        Segment** memory = malloc(sizeof(Segment*) * 32768);
        int memory_top = 0;
        int memory_length = 32768;
        Stack_T ids = Stack_new();
        Segment *seg0;

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

        // Memory_add_segment(mem, pgm_length);
        seg0 = malloc(sizeof(*seg0) + sizeof(uint32_t) * pgm_length);
        seg0->size = pgm_length;

        memory[memory_top] = seg0;
        memory_top++;
        // Seq_addhi(memory, seg0);

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
                // Memory_put(mem, 0, i, instr);
                seg0->data[i] = instr;
        }

        fclose(program);

        while (1) {
                /* fetch */
                instr = seg0->data[pc];
                // printf("instruction is 0x%08x\n", instr);

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
                // printf("ra = %d, contents are %08x\n", ra, reg[ra]);

                Segment *seg = NULL;
                // int length;
                int id;
                switch (opcode) {
                        case CMOV:
                                if (reg[rc] != 0) {
                                        reg[ra] = reg[rb];
                                }                                
                                break;
                        case SLOAD:
                                // reg[ra] = Memory_get(mem, reg[rb], reg[rc]);
                                // seg = Seq_get(memory, reg[rb]);
                                seg = memory[reg[rb]];
                                reg[ra] = seg->data[reg[rc]];
                                break;
                        case SSTORE:
                                // Memory_put(mem, reg[ra], reg[rb], reg[rc]);
                                // seg = Seq_get(memory, reg[ra]);
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
                                // Memory_free(&mem);
                                for (i = 0; i < memory_top; i++) {
                                        // UArray_T seg = (UArray_T)Seq_get((*mem)->memory, i);
                                        // seg = (Segment *)Seq_get(memory, i);
                                        seg = memory[i];
                                        if (seg != NULL)
                                                free(seg);
                                }

                                free(memory);
                                Stack_free(&ids);
                                // printf("expand count is: %d\n", expand_count);
                                exit(EXIT_SUCCESS);
                                break;
                        case MAP: 
                                // reg[rb] = Memory_add_segment(mem, reg[rc]);
                                seg = malloc(sizeof(*seg) + sizeof(uint32_t) * reg[rc]);
                                seg->size = reg[rc];
                                
                                for (i = 0; i < (int)reg[rc]; i++)
                                        seg->data[i] = 0;

                                if (Stack_empty(ids)) {

                                        id = memory_top;
                                        if(id == memory_length){
                                                Segment** mem_new = malloc(sizeof(Segment*) * memory_length*2);
                                                for(i = 0; i < memory_length; i++) {
                                                        mem_new[i] = memory[i];
                                                }
                                                free(memory);

                                                memory = mem_new;
                                                memory_length *= 2;
                                                //TODO: expand
                                        }
                                        memory[id] = seg;
                                        // Seq_addhi(memory, seg);
                                        memory_top++;
                                } else {
                                        id = (int)(uintptr_t)Stack_pop(ids);
                                        memory[id] = seg;
                                        // Seq_put(memory, id, seg);
                                }

                                reg[rb] = id;
                                break;
                        case UNMAP:
                                // Memory_remove_segment(mem, (int)reg[rc]);
                                seg = memory[reg[rc]];
                                free(seg);
                                memory[reg[rc]] = NULL;
                                // Seq_put(memory, reg[rc], NULL);
                                Stack_push(ids, (void *)(uintptr_t)reg[rc]);
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

                                // Memory_load_program(mem, reg[rb]);
                                // seg = Seq_get(memory, reg[rb]);
                                seg = memory[reg[rb]];
                                int prog_len = seg->size;

                                free(seg0);
                                seg0 = malloc(sizeof(*seg0) + sizeof(uint32_t) * prog_len);
                                // Seq_put(memory, 0, seg0);
                                memory[0] = seg0;

                                for (i = 0; i < prog_len; i++) {
                                        seg0->data[i] = seg->data[i];
                                        // Memory_put(mem, 0, i, Memory_get(mem, id, i));
                                }

                                break;
                        default:
                                exit(EXIT_FAILURE);
                }        
        }

        return 0;
}
