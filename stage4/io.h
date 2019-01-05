/*
 * Yang Lowe and Benny Roover, 11/20/18
 * Comp 40 HW 6
 * io.h
 *
 * Header file for io.c. Has access to registers
*/

#ifndef IO_H
#define IO_H

#include <stdio.h>
#include "registers.h"

void IO_input(FILE *stream, reg_name rc);
void IO_output(FILE *stream, reg_name rc);

#endif