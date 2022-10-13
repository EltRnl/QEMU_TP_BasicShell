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
  int i = 0;
  int count = 0;
  uart_send_string(UART0, "\nQuit with \"C-a c\" and then type in \"quit\".\n");
  uart_send_string(UART0, "\nHello world!\n");

  while (1) {
    unsigned char c;
    while (0 == uart_receive(UART0, &c)) {
      // friendly reminder that you are polling and therefore spinning...
      // not good for the planet! But until we introduce interrupts,
      // there is nothing you can do about it... except comment out
      // this annoying code ;-)
      /*
      count++;
      if (count > 10000000) {
        uart_send_string(UART0, "\n\rZzzz....\n\r");
        count = 0;
      }
      */
    }
    if (c == '\r')
      uart_send(UART0, '\n');
    uart_send(UART0, c);
  }
}
