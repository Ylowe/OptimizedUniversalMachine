/*
 * Yang Lowe and Benny Roover, 11/20/18
 * Comp 40 HW 6
 * memory.h
 *
 * Header file for memory.c. Note that Memory_T is defined as a pointer to
 * Memory_T
 */


#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

typedef struct Memory_T *Memory_T;

extern Memory_T  Memory_new();
extern void      Memory_free(Memory_T *memory);
extern int       Memory_add_segment(Memory_T memory, int segment_size);
extern void      Memory_remove_segment(Memory_T memory, int id);
extern void      Memory_put(Memory_T mem, int id, int index, uint32_t val);
extern uint32_t  Memory_get(Memory_T mem, int id, int index);
extern void      Memory_load_program(Memory_T memory, int id);

#endif