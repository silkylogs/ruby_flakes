/* ----- Helpful typedefs ----- */

#include <stdint.h>
#include <stdbool.h>

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef int64_t i64;
typedef int32_t i32;
typedef int16_t i16;

typedef size_t usize;
typedef ssize_t isize;
typedef uintptr_t uintptr;
typedef u32 b32;
typedef uintptr b64;

/* ----- Helpful typedefs ----- */

/* ------ Placeholder cstdlib functions ----- */
int puts(const char *);
int printf(const char *, ...);
void *malloc(usize size);
void free(void *mem_block);
void exit(int);
/* ------ Placeholder cstdlib functions ----- */

#define RF_TEST(ACCUMULATOR_BOOL, CURR_BOOL, EXPRESSION) \
(CURR_BOOL) = (EXPRESSION); \
if (!(CURR_BOOL)) { \
	printf("%s:%d:0: Error: Test failed: %s\n", __FILE__, __LINE__, #EXPRESSION); \
} \
(ACCUMULATOR_BOOL) &= (CURR_BOOL);

#define RF_CAST(VARIABLE, CAST_TO) ((CAST_TO) (VARIABLE))

#include "rf_assert.c"
#include "rf_Array.c"
#include "rf_Memory.c"

/* ----- rf_AsciiString ----- */

struct rf_AsciiString {
	char *chars;
	u64 str_len;
	u64 capacity;
};

b32 rf_AsciiString_test(void);
void rf_AsciiString_from_const_cstr(struct rf_AsciiString *astr, char *const cstr, u64 cstr_len);

b32 rf_AsciiString_test(void) {
	struct rf_AsciiString test;
	b32 all_ok;
	b32 curr_ok;

	all_ok = true;
	curr_ok = true;	
	
	/* Elements must be one byte long */
	RF_TEST(all_ok, curr_ok, (sizeof *test.chars == 1));

	return all_ok;
}

/* ----- rf_AsciiString ----- */

/* ----- rf_main ----- */
b32 rf_run_all_tests(void);
b32 rf_run_all_tests(void);
b32 rf_program_main(void);

b32 rf_run_all_tests(void) {
	b32 all_ok;
	b32 curr_ok;
	
	all_ok = true;
	curr_ok = true;

#define RF_RT(EXPR) RF_TEST(all_ok, curr_ok, (EXPR))
	RF_RT(rf_Array_test());
	RF_RT(rf_AsciiString_test());
	RF_RT(rf_Memory_test());
#undef RF_RT
			    
 	return all_ok;		    
}

b32 rf_program_main(void) {
	/* Main dogma:
	 * 1. Acquire resources
	 * 2. Use resources in game loop
	 * 3. Free resources and return
	 */

	
	
	if (!rf_run_all_tests()) {
		printf("Error: One or more tests failed\n");
		return false;
	}
	
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
