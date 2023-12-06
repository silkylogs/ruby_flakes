int puts(const char *);
int printf(const char *, ...);
void exit(int);

#include <stdint.h>
#include <stdbool.h>

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef int64_t i64;

typedef size_t usize;
typedef ssize_t isize;
typedef uintptr_t uintptr;
typedef u32 b32;
typedef uintptr b64;

#define RF_CHECK_ASSUMPTION(ACCUMULATOR_BOOL, CURR_BOOL, EXPRESSION) \
(CURR_BOOL) = (EXPRESSION); \
if (!(CURR_BOOL)) { \
	printf("%s:%d:0: Warning: Expression evals to false: %s\n", __FILE__, __LINE__, #EXPRESSION); \
} \
(ACCUMULATOR_BOOL) &= (CURR_BOOL);

#define RF_CAST(VARIABLE, CAST_TO) ((CAST_TO) (VARIABLE))

#include "rf_assert.c"
#include "rf_Array.c"

/* ----- rf_Memory ----- */

/* TODO: Recreate memory allocator */

void *malloc(usize size);
void free(void *mem_block);
b32 rf_Memory_test(void);
b32 rf_Memory_test(void) {
	void *backing_buffer;
	usize backing_buffer_len;

	/* All tests ok, current test ok */
	b32 aok = true;
	b32 cok = true;

	/* Init backing buffer */
	backing_buffer_len = 0x400;
	backing_buffer = malloc(backing_buffer_len);
	RF_CHECK_ASSUMPTION(aok, cok, backing_buffer && "Backing buffer failed");
	if (!cok) return false;

	/* Testing overcommit */ {
	}

	free(backing_buffer);
	return aok;
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
b32 rf_run_all_tests(void);
b32 rf_program_main(void);

b32 rf_check_all_assumptions(void) {
	b32 all_ok;
	b32 curr_ok;
	
	all_ok = true;
	curr_ok = true;

#define RF_ASS(EXPR) RF_CHECK_ASSUMPTION(all_ok, curr_ok, (EXPR))
	RF_ASS(rf_Array_check_assumptions());
	RF_ASS(rf_AsciiString_check_assumptions());
	RF_ASS(rf_Memory_test());
#undef RF_ASS
			    
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
		puts("Error: Assumption(s) were violated\n");
		return false;
	}

	/* Try printing a string, the hard way */
	RF_ARRAY_CHECKED_INIT(
		&char_array,
		RF_CAST(text, u8 *),
		sizeof text,
		sizeof text[0],
		sizeof text);
	puts(RF_CAST(char_array.mem_as_bytes, char *));
	
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
