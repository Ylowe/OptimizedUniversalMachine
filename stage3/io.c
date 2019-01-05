/*
 * Yang Lowe and Benny Roover, 11/20/18
 * Comp 40 HW 6
 * io.c
 *
 * IO module for the UM. Takes care of input and output through the C stdio
 * library
*/

#include "io.h"

/*
* Function: Input
* @stream - input stream
* @rc     - register index which will store the input
*
* Gets a byte from the input stream and puts it in reg[rc]. If the byte is
* an EOF character, then store 32 activated bits in reg[rc]
*/
void IO_input(FILE *stream, reg_name rc)
{
	char input = getc(stream);
	if (input == EOF) {
		reg[rc] = ~0;
	}
	else {
		reg[rc] = input;
	}
}

/*
* Function: Output
* @stream - input stream
* @rc     - register index which will be outputted
*
* Outputs the data in reg[c] to the stream
*/
void IO_output(FILE *stream, reg_name rc)
{
	putc((uint8_t)reg[rc], stream);
}