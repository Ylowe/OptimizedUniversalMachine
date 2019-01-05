/*
 * Yang Lowe and Benny Roover, 11/20/18
 * Comp 40 HW 6
 * registers.h
 *
 * defines a typedef for eight register names r0-r7, and declares an external
 * array of eight uint32_t's representing registers. The actual register
 * variable is initialized in um.c
 */

#ifndef REGISTERS_H
#define REGISTERS_H

#include <stdint.h>

typedef enum reg_name { r0 = 0, r1, r2, r3, r4, r5, r6, r7 } reg_name;
extern uint32_t reg[8]; /* references global register array defined in um.c */

#endif