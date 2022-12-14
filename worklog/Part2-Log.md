# Qemu TP Part 2 - Interupts & Events
-> This is the worklog for Part 2 of the Qemu TP, detailing what I do along the way.

## Commit 1 - First steps
This was way harder that I thought and I took too much time not start understanding the goal.

### Compiling
Modified the Makefile to handle the new files, and also added the definition of irq_stack_top to the memory map as I saw it needed it.

Had to have a specific instruction in the makefile for .S files like irqs.S.

### First steps for the interrupts
Understoof quickly that I had to have code in _irq_handler in vector.s, I took the code I found on the slides to call the function irs.

I figured out how the functions in vic.h/vic.c works a bit, using a table of handlers for the different interrupts.

After consulting with classmates I understand a bit more the use of the declarations in uart-irqs.h, and am starting to use them to enable the interrupts.

I have a function uart_init to do just that, for now it only enables Receive Interrupts.

### Current situation
The code compiles, prints out the first things in my command line but when I type a charcter, it restarts my shell, probably because it causes an undefined instruction as it continue to do it after this and keeps trying to restart.

### Work for next commit
Read the doc more intensely, especially the part contained in uart-irqs.h so that I can understand what all this means. Try and find out what my code is trully doing right now. Add the circular buffer to the code.

## Commit 2 - Debugging session

### Uncovered Problems
Typing a character does not trigger the _irq_handler, but the _undefined_instruction. However, if I type a character before starting my loop, the interrupt trigger as expected. This is very confusing and I don't understand what causes it yet. There is probably something wrong with my main loop in shell.c.

### A bigger problem ?
Weirder problem, if I switch from 'versatilepb' (that I've been using so far by accident) to 'versatileab', calling anything outside of the file main.c causes an undefined instruction to occure, so there might be a bigger underlying problem happening right now.

## Commit 3 - Big Problem Gone

### False alarm

I just realized that 'versatileab' isn't suppose to work with interrupts -_- so my previous worry was not a problem, I'll stay with 'versatilepb' from now as I did before.

### Found the problem

The weird problem I had where unexpected instructions where raised was actually due to my buffer in shell. I didn't initialized **line_buffer**, I just declared it **"unsigned char* line_buffer"**, as such, the variable **line_buffer** was in the stack, but it's value was not, and it wasn't initialized, as such, when I reseted the buffer it caused problems. 

To resolve this I initialized it like so : **unsigned char line_buffer[BUFFER_SIZE]**

By doing this, the buffer is now in the stack with a fixed size.

### What I need to do

I have to change the shell a tiny bit to take care of that **BUFFER_SIZE**, and changing a few things regarding **max_index** as it is causing me errors with the back space.

## Commit 4 - Some adjustement in the shell

Having changed the buffer manipulation earlier, I had to change a few things regarding how I handle **max_index**, changing its value when looking at what character I type.

I also made it so that we cannot type additional character once we are at the character limit.