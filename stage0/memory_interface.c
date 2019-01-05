/*
 * Yang Lowe and Benny Roover, 11/20/18
 * Comp 40 HW 6
 * memory_interface.h
 *
 * implementation for memory interface instructions. These call functions
 * from memory.c and change the values of the registers and/or memory.
 */

#include "memory_interface.h"
#include "memory.h"

/* 
 * note - all of these functions have the same arguments:
 * @mem - Memory_T type representing the segmented memory
 * @ra, @rb, @rc - names of registers to access
 */

/*
 * function MEM_seg_load
 * This function exeutes the segmented load instruction, and it stores the
 * value of the mem[$rb][$rc] to $ra.
 */
void MEM_seg_load(Memory_T mem, reg_name ra, reg_name rb, reg_name rc)
{
	reg[ra] = *Memory_at(mem, reg[rb], reg[rc]);
}

/* 
 * function MEM_seg_str
 * This function executes the segmented store instruction, and it stores the
 * value $ra at the memory location mem[$rb][$rc]
 */
void MEM_seg_str(Memory_T mem, reg_name ra, reg_name rb, reg_name rc)
{
        *Memory_at(mem, reg[ra], reg[rb]) = reg[rc];
}

/*
 * function MEM_map_seg
 * This function exeutes the Map instruction, and it maps a new segment
 * to the memory with size $rc and saves the segment's id to $rb.
 */
void MEM_map_seg(Memory_T mem, reg_name rb, reg_name rc)
{
        reg[rb] = Memory_add_segment(mem, reg[rc]);
}

/* 
 * function MEM_unmap_seg
 * This function exeutes the Unmap instruction, and it unmaps the segment with
 * the id $rc.
 */
void MEM_unmap_seg(Memory_T mem, reg_name rc)
{
        Memory_remove_segment(mem, (int)reg[rc]);
}

/* 
 * function MEM_load_pgm
 * This function executes the Load Program instruction. If the id of the
 * segment to load is not 0, it copies that segment to segment 0.
 * The function returns the value of rc, which is saved as the new
 * program counter in um.c's execute function.
 */
uint32_t MEM_load_pgm(Memory_T mem, reg_name rb, reg_name rc)
{
	if (reg[rb] != 0) {
	        Memory_load_program(mem, reg[rb]);		
	}
	
        return reg[rc];
}