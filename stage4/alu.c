/*
 * Yang Lowe and Benny Roover, 11/20/18
 * Comp 40 HW 6
 * alu.c
 *
 * ALU module for the UM. Contains all logic and arithmetic operations. All
 * functions except loadval takes three register indexes: ra, rb, and rc.
*/

#include "alu.h"

/*
* Function: Conditional Move
* Moves the data in reg[rb] to reg[ra] if reg[rc] is not 0
*/
void ALU_cmov(reg_name ra, reg_name rb, reg_name rc)
{
	if (reg[rc] != 0) {
		reg[ra] = reg[rb];
	}
}

/*
* Function: Add
* Adds the data at reg[rb] and reg[rc]. Modulates the result by 2^32 and stores
* it in reg[ra]
*/
void ALU_add(reg_name ra, reg_name rb, reg_name rc)
{
	reg[ra] = (reg[rb] + reg[rc]) & ~0UL;
}

/*
* Function: Multiply
* Multiplies the data at reg[rb] and reg[rc]. Modulates the result by 2^32 and 
* stores it in reg[ra]
*/
void ALU_mul(reg_name ra, reg_name rb, reg_name rc)
{
	reg[ra] = (reg[rb] * reg[rc]) & ~0UL;
}

/*
* Function: Divide
* Divides the data at reg[rb] by reg[rc]. Stores the result in reg[ra]. Integer
* division.
*/
void ALU_div(reg_name ra, reg_name rb, reg_name rc)
{
	reg[ra] = (reg[rb] / reg[rc]);
}

/*
* Function: NAND
* Ands the bits stored at reg[rb] with reg[rc]. Flips the bits and stores the 
* result in ra
*/
void ALU_nand(reg_name ra, reg_name rb, reg_name rc)
{
	reg[ra] = ~(reg[rb] & reg[rc]);
}

/*
* Function: Multiply
* @ra  - index of register a
* @val - value to be stored in the target memory index
* 
* Stores val in reg[ra]
*/
void ALU_loadval(reg_name ra, uint32_t val)
{
	reg[ra] = val;
}