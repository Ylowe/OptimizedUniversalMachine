/*
 * Yang Lowe and Benny Roover, 11/20/18
 * Comp 40 HW 6
 * memory.c
 *
 * Memory interface using a hanson array and sequence. Sequence serves as the
 * "backbone" of the memory ADT, and the UArrays are the segments stored in
 * the Sequence. Hanson stack used as a helper structure to store avialable 
 * sequence IDs. Stores exclusively uint32_ts.
 */


#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <seq.h>
#include <uarray.h>
#include <assert.h>
#include <stack.h>
#include "memory.h"

struct Memory_T
{
	Seq_T memory;
	Stack_T ids;
};

/*
* Function: New
* No arguments
*
* Mallocs memory for the struct ADT type. Initiates the sequence and stack.
* Memory T is defined as a pointer to Memory_T. Returns a pointer to the
* initilized Memory_T
*/
Memory_T Memory_new()
{
	Memory_T mem = malloc(sizeof(*mem));
	mem->memory = Seq_new(10);
	mem->ids = Stack_new();
	return mem;
}

/*
* Function: Free
* @mem - pointer to a Memory_T
*
* Deallocates all segments allocated in the sequence, and then frees the
* sequence and the stack. Frees the pointer to memory and sets it to NULL
*/
void Memory_free(Memory_T *mem) {
	int length = Seq_length((*mem)->memory);
	int i;

	for (i = 0; i < length; i++) {
		UArray_T seg = (UArray_T)Seq_get((*mem)->memory, i);
		if (seg != NULL)
			UArray_free(&seg);
	}

	Seq_free(&((*mem)->memory));
	Stack_free(&((*mem)->ids));
	free(*mem);
	*mem = NULL;
}

/*
* Function: Add segment
* @mem - A Memory_T object
* @seg_length - Number of uint32 words to be allocated
*
* Mallocs space for a new UArray and assigns it an ID. The ID is the value
* popped off of the stack. If the stack is empty, the ID is equal to
* the length of the sequence. Function returns the ID.
*/
int Memory_add_segment(Memory_T mem, int seg_length)
{
	UArray_T seg = UArray_new(seg_length, sizeof(uint32_t));
	int id;

	if (Stack_empty(mem->ids)) {
		id = Seq_length(mem->memory);
		Seq_addhi(mem->memory, seg);
	} else {
		id = (int)(uintptr_t)Stack_pop(mem->ids);
		Seq_put(mem->memory, id, seg);
	}

	return id;
}

/*
* Function: Remove segment
* @mem - A Memory_T object
* @id  - ID of the segment to be removed
*
* Gets the id of the target segment, and frees the UArray that the pointer
* points to. Sets the pointer at that segment index to NULL, and pushes the
* ID to the stack as a uintptr_t.
*/
void Memory_remove_segment(Memory_T mem, int id)
{
	UArray_T seg = (UArray_T)Seq_get(mem->memory, id);
	UArray_free(&seg);
	Seq_put(mem->memory, id, NULL);
	Stack_push(mem->ids, (void *)(uintptr_t)id);
}

/*
* Function: At
* @mem   - A Memory_T object
* @id    - Index relative to the Sequence
* @index - Index relative to the UArray
*
* Returns a pointer to the item at segment of [ID] and index of [index]
*/
uint32_t* Memory_at(Memory_T mem, int id, int index)
{
	return (uint32_t*)UArray_at((UArray_T)Seq_get(mem->memory, id), 
								index);
}

/*
* Function: Load program
* @mem - A Memory_T object
* @id  - ID of the segment to be loaded
*
* Frees the data at segment 0, and creates a new segment at segment 0 which
* is the same length as the target segment. Copies over the data from
* the target segment to segment 0
*/
void Memory_load_program(Memory_T mem, int id)
{
	UArray_T seg0 = (UArray_T)Seq_get(mem->memory, 0);
	UArray_T program = (UArray_T)Seq_get(mem->memory, id);
	int prog_len = UArray_length(program);
	int i;

	UArray_free(&seg0);
	Seq_put(mem->memory, 0, UArray_new(prog_len, sizeof(uint32_t)));

	for (i = 0; i < prog_len; i++) {
		*Memory_at(mem, 0, i) = *Memory_at(mem, id, i);
	}
}

/*********** TESTING CODE BELOW ***************/

#define PASS true
#define FAIL false

void exit_failure();
bool test_allocate_and_free();
bool test_add_and_remove_segment();
bool test_at();
bool test_load_program();

/*
int main()
{
	if (test_allocate_and_free() == FAIL)
		exit_failure();
	if (test_add_and_remove_segment() == FAIL)
		exit_failure();
	if (test_at() == FAIL)
		exit_failure();
	if (test_load_program() == FAIL)
		exit_failure();

	printf("all tests passed!\n");
	return 0;
}
*/

void exit_failure()
{
	printf("test failed!\n");
	exit(1);
}

bool test_allocate_and_free()
{
	/* allocate a new Memory_T, make sure it's not NULL */
	Memory_T mem = Memory_new();

	if (mem == NULL)
		return FAIL;

	/* free the Memory_T, make sure it is NULL */
	Memory_free(&mem);

	if (mem != NULL)
		return FAIL;

	return PASS;
}

bool test_add_and_remove_segment()
{
	Memory_T mem = Memory_new();
	assert(mem != NULL);
	int id = 0;
	// note: test freeing unmapped segments
	/* add a segment */
	id = Memory_add_segment(mem, 2048);

	if (id != 0)
		return FAIL;
	if (UArray_length(Seq_get(mem->memory, id)) != 2048)
		return FAIL;
	printf("added a segment\n");
	/* add a second segment */
	id = Memory_add_segment(mem, 1024);
	if (id != 1)
		return FAIL;
	if (UArray_length(Seq_get(mem->memory, id)) != 1024)
		return FAIL;
	printf("added a second segment\n");

	/* remove last segment */
	Memory_remove_segment(mem, id);
	if (Stack_empty(mem->ids))
		return FAIL;
	printf("removed last segment\n");

	/* check that the sequence at the segment id points to NULL */
	if (Seq_get(mem->memory, id) != NULL)
		return FAIL;

	printf("segment is null\n");
	/* popped id should be 1 */
	id = (int)(uintptr_t)Stack_pop(mem->ids);
	
	if (id != 1)
		return FAIL;
	Stack_push(mem->ids, (void *)(uintptr_t)id);
	printf("popped id is %d\n", id);

	/* add a new segment, should have id = 1 */
	id = Memory_add_segment(mem, 4096);
	if (id != 1)
		return FAIL;
	if (!Stack_empty(mem->ids))
		return FAIL;
	printf("added another segment with id\n");
	Memory_free(&mem);
	return PASS;
}

bool test_at()
{
	Memory_T mem = Memory_new();
	assert(mem != NULL);

	int id = Memory_add_segment(mem, 1024);
	uint32_t value = 58;
	uint32_t new_val;
	int index = 1;

	*Memory_at(mem, id, index) = value;

	new_val = *Memory_at(mem, id, index);
	if (value != new_val)
		return FAIL;

	Memory_free(&mem);
	return PASS;
}

bool test_load_program()
{
	Memory_T mem = Memory_new();
	assert(mem != NULL);

	int id0 = Memory_add_segment(mem, 10);
	printf("id0 is %d\n", id0);
	int id1 = Memory_add_segment(mem, 20);
	int i;

	*Memory_at(mem, id1, 1) = 41;
	*Memory_at(mem, id1, 3) = 401;
	*Memory_at(mem, id1, 16) = 583928;

	for (int i = 0; i < 10; i++)
		printf("%d ", *Memory_at(mem, 0, i));
	printf("\n");
	for (int i = 0; i < 20; i++)
		printf("%d ", *Memory_at(mem, id1, i));
	printf("\n");
	Memory_load_program(mem, id1);

	/* check that length of segment 0 is 2048 */
	if (UArray_length(Seq_get(mem->memory, id0)) != 20)
		return FAIL;

	/* check that length of segment 1 is 2048 */
	if (UArray_length(Seq_get(mem->memory, id1)) != 20)
		return FAIL;

	/* check each element in seg 0 matches each element in seg 1 */
	for (i = 0; i < 20; i++) {
		if (*Memory_at(mem, id0, i) != *Memory_at(mem, id1, i))
			return FAIL;
	}

	for (int i = 0; i < 20; i++)
		printf("%d ", *Memory_at(mem, 0, i));
	printf("\n");
	for (int i = 0; i < 20; i++)
		printf("%d ", *Memory_at(mem, id1, i));
	printf("\n");

	int id2 = Memory_add_segment(mem, 25);
	*Memory_at(mem, id2, 2) = 11;
	*Memory_at(mem, id2, 7) = 4021;
	*Memory_at(mem, id2, 15) = 5828;

	Memory_load_program(mem, id2);

	for (int i = 0; i < 25; i++)
		printf("%d ", *Memory_at(mem, 0, i));
	printf("\n");
	for (int i = 0; i < 25; i++)
		printf("%d ", *Memory_at(mem, id2, i));
	printf("\n");

	Memory_free(&mem);
	return PASS;
}