#include "main.h"

/**
 * This is the C entry point, upcalled once the hardware has been setup properly
 * in assembly language, see the reset.s file.
 */

// Adding some global variables to check the bss section along with initialized ones to see where they end up
void* test0;
void* test1;
void* test2 = 14;
void* test3;
void* test4 = 1456;
void* test5;
void* test6;
void* test7;
void* test8;
void* test9 = 42;

void _start() {
	shell();
}
