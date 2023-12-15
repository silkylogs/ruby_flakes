#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define SDL_MAIN_HANDLED
#include "../external_includes/SDL2/SDL.h"

/* ----- Helpful typedefs ----- */
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

typedef signed int i32;
typedef signed short i16;
typedef signed char i8;

typedef unsigned long int usize;
typedef signed long int isize;
typedef u32 b32;
/* ----- Helpful typedefs ----- */

#define RF_TEST(ACCUMULATOR_BOOL, CURR_BOOL, EXPRESSION)		\
	(CURR_BOOL) = (EXPRESSION);					\
	if (!(CURR_BOOL)) {						\
	printf(								\
		"%s:%d:0: Error: Test failed: %s\n",			\
		__FILE__, RF_CAST(__LINE__, i32), #EXPRESSION);		\
	}								\
	(ACCUMULATOR_BOOL) &= (CURR_BOOL);

#define RF_CAST(VARIABLE, CAST_TO) ((CAST_TO) (VARIABLE))

#include "rf_Assert.c"
#include "rf_Array.c"
#include "rf_AsciiString.c"
#include "rf_Memory.c"

/* ----- rf_main ----- */
b32 rf_run_all_tests(void);
b32 rf_run_all_tests(void);
b32 rf_program_main(void);

b32 rf_run_all_tests(void) {
	b32 all_ok = true;
	b32 curr_ok = true;
	
	printf("\n\n"
	       "Running all tests now\n"
	       "Please don't panic if you see warning messages within this region\n"
	       "---------------------------------------------------------------------------\n");
	
	RF_TEST(all_ok, curr_ok, rf_Array_test());
	RF_TEST(all_ok, curr_ok, rf_AsciiString_test());
	RF_TEST(all_ok, curr_ok, rf_Memory_test());
	
	printf("---------------------------------------------------------------------------\n"
	       "Tests are over, start panicking\n"
	       "\n\n");

 	return all_ok;		    
}

b32 rf_program_main(void) {
	/* Main dogma:
	 * 1. Acquire resources
	 * 2. Use resources in game loop
	 * 3. Free resources and return
	 */

	SDL_Window *window;
	SDL_GLContext context;
	SDL_Event event;
	b32 running;

	if (!rf_run_all_tests()) {
		printf("Error: One or more tests failed\n");
		return false;
	}

	/* Testing SDL */
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow(
		"Test",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		400, 400, SDL_WINDOW_SHOWN);

	SDL_Delay(1000);	
	SDL_DestroyWindow(window);
	SDL_Quit();
	
	return true;
}

/* ----- rf_main ----- */

void rf_use_argc_and_argv(int argc, char **argv);
void rf_use_argc_and_argv(int argc, char **argv){
	printf("Argument count: %d, args[0] = %s\n", argc, argv[0]); 
}

int main(int argc, char **args) {
	b32 retval;
	retval = rf_program_main();
       	printf("Program exited normally\n");

	rf_use_argc_and_argv(argc, args);
	
	/* Converting true/false value into proper return value */
	retval = (retval == true)? 0 : retval;
	return retval;
}

