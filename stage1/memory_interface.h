/*
 * Yang Lowe and Benny Roover, 11/20/18
 * Comp 40 HW 6
 * memory_interface.h
 *
 * defines functions for the memory interface instructions. These are
 * essentially wrappers for the memory.c functions, and they modify
 * or access data from the segmented memory.
 */

#ifndef MEMORY_INTERFACE_H
#define MEMORY_INTERFACE_H

#include <stdint.h>
#include "registers.h"
#include "memory.h"

void     MEM_seg_load(Memory_T mem, reg_name ra, reg_name rb, reg_name rc);
void     MEM_seg_str(Memory_T mem, reg_name ra, reg_name rb, reg_name rc);
void     MEM_map_seg(Memory_T mem, reg_name rb, reg_name rc);
void     MEM_unmap_seg(Memory_T mem, reg_name rc);
uint32_t MEM_load_pgm(Memory_T mem, reg_name rb, reg_name rc);

#endif