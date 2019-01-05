/*
 * Yang Lowe and Benny Roover, 11/20/18
 * Comp 40 HW 6
 * alu.h
 *
 * header file for alu.c. Has access to registers.
*/

#ifndef ALU_H
#define ALU_H

#include <stdint.h>
#include "registers.h"

void ALU_cmov(reg_name ra, reg_name rb, reg_name rc);
void ALU_add(reg_name ra, reg_name rb, reg_name rc);
void ALU_mul(reg_name ra, reg_name rb, reg_name rc);
void ALU_div(reg_name ra, reg_name rb, reg_name rc);
void ALU_nand(reg_name ra, reg_name rb, reg_name rc);
void ALU_loadval(reg_name ra, uint32_t val);

#endif