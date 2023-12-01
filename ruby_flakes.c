int puts(const char *);
int printf(const char *, ...);

#include <stdint.h>
#include <stdbool.h>
typedef uint64_t u64;
typedef uint32_t b32;

#define RF_CHECK_ASSUMPTION(ACCUMULATOR_BOOL, CURR_BOOL, EXPRESSION) \
(CURR_BOOL) = (EXPRESSION); \
if (!(CURR_BOOL)) { \
	printf("Warning: Assumption failed: %s, line: %d\n", #EXPRESSION, __LINE__); \
} \
(ACCUMULATOR_BOOL) &= (CURR_BOOL);

/* ----- rf_Array ----- */

struct rf_Array {
	void *elems;
	u64 elem_len;
	u64 arr_len;
};

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
struct rf_Memory {
	struct rf_Array memory;
};

struct rf_Memory_fat_ptr;

/*
void *rf_Memory_alloc(struct rf_Memory *mem, u64 size);
b32 rf_Memory_is_ptr_valid(struct rf_Memory *mem);
void rf_Memory_free(void*);
*/

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
