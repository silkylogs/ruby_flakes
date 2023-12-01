int puts(const char *);
int printf(const char *, ...);

#include <stdint.h>
#include <stdbool.h>
typedef uint64_t u64;
typedef int64_t i64;
typedef uint32_t b32;

#define RF_CHECK_ASSUMPTION(ACCUMULATOR_BOOL, CURR_BOOL, EXPRESSION) \
(CURR_BOOL) = (EXPRESSION); \
if (!(CURR_BOOL)) { \
	printf("Warning: Assumption failed: %s, line: %d\n", #EXPRESSION, __LINE__); \
} \
(ACCUMULATOR_BOOL) &= (CURR_BOOL);

#define RF_CAST(VARIABLE, CAST_TO) ((CAST_TO) (VARIABLE))

/* ----- rf_Array ----- */

struct rf_Array {
	void *elems;
	u64 elem_len;
	u64 arr_len;
};

/* TODO: maybe use ranged pointers to express subarrays? */
struct rf_Memory_fat_ptr;

b32 rf_Array_check_assumptions(void);
void rf_Array_init(struct rf_Array *array, void *new_elems, u64 new_elem_length, u64 new_arr_length);

b32 rf_Array_check_assumptions() {
	b32 all_ok;
	b32 curr_ok;
	struct rf_Array test;

	all_ok = true;
	curr_ok = true;
	
	/* Type length check */
	RF_CHECK_ASSUMPTION(all_ok, curr_ok, (sizeof test.elem_len == 8));
	RF_CHECK_ASSUMPTION(all_ok, curr_ok, (sizeof test.arr_len == 8));
        
	return all_ok;
}

void rf_Array_init(struct rf_Array *array, void *new_elems, u64 new_elem_length, u64 new_arr_length) {
	array->elems = new_elems;
	array->elem_len = new_elem_length;
	array->arr_len = new_arr_length;
}

/* ----- rf_Array ----- */

/* ----- rf_Memory ----- */

/*
  TODO: implement this as a free list allocator
  reference: https://www.gingerbill.org/article/2021/11/30/memory-allocation-strategies-005/
*/

struct rf_Memory_FreeListAllocationHeader {
	u64 block_size;
	u64 padding;
};

struct rf_Memory_FreeListNode {
	struct rf_Memory_FreeListNode *next;
	u64 block_size;
};

enum rf_Memory_PlacementPolicy {
	RF_MEMORY_PLACEMENT_POLICY_FIND_FIRST,
	RF_MEMORY_PLACEMENT_POLICY_FIND_BEST
};

struct rf_Memory {
        void *data;
	i64 used;
	i64 size;

	struct rf_Memory_FreeListNode *head;
	enum rf_Memory_PlacementPolicy policy;
};

void rf_Memory_free_all(struct rf_Memory *rf_mem);
void rf_Memory_free_all(struct rf_Memory *rf_mem) {
	struct rf_Memory_FreeListNode *first_node;
	
	rf_mem->used = 0;
	first_node = rf_mem->data;
	first_node->block_size = rf_mem->size;
	first_node->next = NULL;
	rf_mem->head = first_node;
}

void rf_Memory_init_from_array(struct rf_Memory *rf_mem, void *new_data, i64 new_data_len);
void rf_Memory_init_from_array(struct rf_Memory *rf_mem, void *new_data, i64 new_data_len) {
	rf_mem->data = new_data;
	rf_mem->size = new_data_len;
	rf_Memory_free_all(rf_mem);
}

u64 rf_Memory_calculate_padding_with_header(uintptr_t ptr, uintptr_t alignment, u64 header_size);
u64 rf_Memory_calculate_padding_with_header(uintptr_t ptr, uintptr_t alignment, u64 header_size) {
	uintptr_t p, a, modulo, padding, needed_space;

	/* assert alignment is power of 2 */
	if (alignment & 0x1)
		return 0;
	
	p = ptr;
	a = alignment;
	modulo = p & (a - 1);
	
	padding = 0;
	needed_space = 0;
	
	if (modulo != 0)
		padding = a - modulo;

	needed_space = header_size;

	if (padding < needed_space) {
		needed_space -= padding;
		if (((needed_space & (a - 1)) != 0)) {
			padding += a * (1 + (needed_space / a));
		} else {
			padding += a * (needed_space / a);
		}
	}

	return padding;
}

struct rf_Memory_FreeListNode *rf_Memory_find_first(
	struct rf_Memory *rf_mem,
	u64 size,
	u64 alignment,
	u64 *arg_padding,
	struct rf_Memory_FreeListNode **arg_prev_node);
struct rf_Memory_FreeListNode *rf_Memory_find_first(
	struct rf_Memory *rf_mem,
	u64 size,
	u64 alignment,
	u64 *arg_padding,
	struct rf_Memory_FreeListNode **arg_prev_node)
{
	/* Iterate the list and find the first free block with enough space */
	struct rf_Memory_FreeListNode *node, *prev_node;
	u64 padding, required_space;

	node = rf_mem->head;
	prev_node = NULL;
	padding = 0;

	while (node != NULL) {
		padding = rf_Memory_calculate_padding_with_header(
			RF_CAST(node, uintptr_t),
			RF_CAST(alignment, uintptr_t),
			sizeof(struct rf_Memory_FreeListAllocationHeader));
		required_space = size + padding;
		if (node->block_size >= required_space) break;
		prev_node = node;
		node = node->next;
	}

	if (arg_padding) *arg_padding = padding;
	if (arg_prev_node) *arg_prev_node = prev_node;
	return node;
}

struct rf_Memory_FreeListNode *rf_Memory_find_best(
	struct rf_Memory *rf_mem,
	u64 size,
	u64 alignment,
	u64 *arg_padding,
	struct rf_Memory_FreeListNode **arg_prev_node);
struct rf_Memory_FreeListNode *rf_Memory_find_best(
	struct rf_Memory *rf_mem,
	u64 size,
	u64 alignment,
	u64 *arg_padding,
	struct rf_Memory_FreeListNode **arg_prev_node)
{
	struct rf_Memory_FreeListNode *node, *prev_node, *best_node;
	u64 smallest_diff, padding, required_space;

	smallest_diff = ~(RF_CAST(0, u64)); /* -1 */

	node = rf_mem->head;
	prev_node = NULL;
	best_node = NULL;
	
	padding = 0;

	while (node != NULL) {
		padding = rf_Memory_calculate_padding_with_header(
			RF_CAST(node, uintptr_t),
			RF_CAST(alignment, uintptr_t),
			sizeof(struct rf_Memory_FreeListAllocationHeader));
		required_space = size + padding;
		if (
			node->block_size >= required_space &&
			/* it.block_size? */
			(node->block_size - required_space < smallest_diff))
		{
			best_node = node;
		}
		prev_node = node;
		node = node->next;
	}

	if (arg_padding) *arg_padding = padding;
	if (arg_prev_node) *arg_prev_node = prev_node;
	return best_node;
}


/* ----- rf_Memory ----- */

/* ----- rf_AsciiString ----- */

struct rf_AsciiString {
	char *chars;
	u64 str_len;
	u64 capacity;
};

b32 rf_AsciiString_check_assumptions(void);
void rf_AsciiString_from_const_cstr(struct rf_AsciiString *astr, char *const cstr, u64 cstr_len);

b32 rf_AsciiString_check_assumptions(void) {
	struct rf_AsciiString test;
	b32 all_ok;
	b32 curr_ok;

	all_ok = true;
	curr_ok = true;	
	
	/* Elements must be one byte long */
	RF_CHECK_ASSUMPTION(all_ok, curr_ok, (sizeof *test.chars == 1));

	return all_ok;
}

/* ----- rf_AsciiString ----- */

/* ----- rf_main ----- */
b32 rf_check_all_assumptions(void);
b32 rf_program_main(void);

b32 rf_check_all_assumptions(void) {
	b32 all_ok;
	b32 curr_ok;
	
	all_ok = true;
	curr_ok = true;
	
	RF_CHECK_ASSUMPTION(all_ok, curr_ok, rf_Array_check_assumptions());
	RF_CHECK_ASSUMPTION(all_ok, curr_ok, rf_AsciiString_check_assumptions());
	
 	return all_ok;
}

b32 rf_program_main(void) {
	/* Main dogma:
	 * 1. Acquire resources
	 * 2. Use resources in game loop
	 * 3. Free resources and return
	 */

	struct rf_Array char_array;
	char text[] = "Goodbye, cruel world!\n";
	
	if (!rf_check_all_assumptions()) {
		puts("Warning: Assumptions are violated\n");
		return false;
	}

	/* Try printing a string, the hard way */
	rf_Array_init(
		&char_array,
		text,
		sizeof text[0],
		sizeof text);
	puts(char_array.elems);
	
	return true;
}

/* ----- rf_main ----- */

int main(void) {
	b32 retval;
	retval = rf_program_main();
       	puts("Exiting program\n");

	/* Converting true/false value into proper return value */
	retval = (retval == true)? 0 : retval;
	return retval;
}
