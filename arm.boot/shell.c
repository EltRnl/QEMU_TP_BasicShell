#include "shell.h"
#include "kprintf.c"
#include "uart.h"
#include "vic.h"
#include "uart-irqs.h"
#include "cb.h"

/***************** UART Events *****************/



/***************** Get/Put Char/String *****************/

/**
 * @brief Used to call 'uart_receive' on UART0, receiving a single char from the command line.
 * 
 * @param c A pointer to where the received value is stored.
 * @return int The flag returned by 'uart_receive' (see uart.h for more precision).
 */
int receive_char(struct cb * rxcb,unsigned char* c){
	return cb_get(rxcb,c)==0?1:0;
}

/**
 * @brief Prints a string of character to the command line (through UART0) from a buffer.
 * 
 * @param str The buffer containing the characters to print.
 */
void print_string(char* str){
	for(int i=0; str[i]!='\0'; i++) kputchar(str[i]);
}

/**
 * @brief Clear the command line using the regex expression "\e[1;1H\e[2J".
 * 
 */
void clear_screen(){
	kprintf("\e[1;1H\e[2J");
}

/***************** Buffer Manipulation *****************/

/**
 * @brief Shift the indexes of a buffer to the left from the element after the index 'pos'.
 * 
 * @example shift_left(['0','1','2','3','4','5','6','7','8','9'], 3) ==> ['0','1','2','4','5','6','7','8','9']
 * 
 * @param buffer The buffer to modify.
 * @param pos The index from which to apply the shift.
 */
void shift_left(char* buffer, int pos){
	for (int i=pos; buffer[i+1]!='\0'; i++){
		buffer[i] = buffer[i+1];
		buffer[i+1] = '\0';
	}
}

/**
 * @brief Shift the indexes of a buffer to the right from the element at index 'pos'.
 * 
 * @example shift_right(['0','1','2','3','4','5','6','7','8','9'], 3) ==> ['0','1','2','\0','3','4','5','6','7','8','9']
 * @warning Considering how we handle strings here, the character at index 'pos' needs to be manualy change afterwards.
 * 
 * @param buffer The buffer to modify.
 * @param pos The index from which to apply the shift.
 */
void shift_right(char* buffer, int pos){
	char tmp1 = buffer[pos];
	char tmp2;
	buffer[pos] = '\0';
	for(int i=pos+1; tmp1!='\0'; i++){
		tmp2 = buffer[i];
		buffer[i] = tmp1;
		tmp1 = tmp2;
	}
}

/**
 * @brief Puts every value in the buffer to '\0'.
 * 
 * @param buffer The buffer to modify.
 */
void reset_buffer(unsigned char* buffer){
	for (int i=0; buffer[i]!='\0'; i++) buffer[i] = '\0';
}

/**
 * @brief Inserts a char in the middle of a buffer, shifting it accordingly using 'shift_right'.
 * 
 * @param buffer The buffer in which we insert the character.
 * @param index The position of the character in the buffer.
 * @param c The character to insert.
 */
void insert_char(char* buffer, int index, char c){
	shift_right(buffer,index);
	buffer[index]=c;
}

/**
 * @brief Checks if 2 buffers are "equal", in the sense that they have the same value up until a certain index.
 * 
 * @param b1 The first buffer to compare.
 * @param b2 The second buffer to compare.
 * @param until The index up to which we check if the buffers have the same value, if we want to compare the whole buffers (until both are '\0'), then we can put until<0.
 * @return int Results of the comparison, 1 if it is true, 0 otherwise.
 */
int buffer_equals(unsigned char* b1, unsigned char* b2, int until){
	for(int i=0; i!=until && (b1[i]!='\0' || b2[i]!='\0'); i++) 
		if(b1[i]!=b2[i]) return 0;
	return 1;
}

/**
 * @brief Calculate the length of a buffer.
 * 
 * @param buffer The buffer we want to know the length of.
 * @return int The size of the buffer.
 */
int len(char* buffer){
	int res;
	for(res = 0; buffer[res]!='\0'; res++);
	return res;
}

/***************** Key-Press Reaction Function *****************/

/**
 * @brief Called when we detect the ecape character of value 0x1b, corresponding to the escape key, the delete key, or one of the arrow keys.
 * 
 * @param buffer The current line buffer.
 * @param index The pointer to the current line index.
 * @return int A flag to show what happened (1: behavior executed succesfully, 0: unexpected character, -1: unimplemented behavior).
 */
int escape_key(struct cb* rxcb, char* buffer, int* index){
	unsigned char c;
	if (0 == receive_char(rxcb, &c) || c!='[') return 0;
	receive_char(rxcb, &c);
	switch (c){
		case '3':	// Del key
			receive_char(rxcb, &c);
			if(c!='~') return 0;
			buffer[*index] = '\0';
			shift_left(buffer,*index);
			break;
		case 'A':	// Up
			break;
		case 'B':	// Down
			break;
		case 'C':	// Right
			if(buffer[*index]!='\0') (*index)++;
			break;
		case 'D':	// Left
			*index = (*index)-1<0?0:((*index)-1);
			break;
		default:
			return -1;
			break;
	}
	return 1;
}

/**
 * @brief Read the line_buffer to exectute the command typed by the user.
 * 
 * @param buffer The current line buffer.
 * @return int A flag to show what happened (1: command 'echo' seen and executed, 0: command 'reset' seen and executed, -1: unimplemented behavior).
 */
int parse_line(unsigned char* buffer){
	/* Not efficient to use 'buffer_equals' for each command, but considering we only have 2 commands it is enough. */
	if(1 == buffer_equals(buffer,"reset",5)){
		clear_screen();
		return 0;
	}
	if(1 == buffer_equals(buffer,"echo",4)){
		kprintf("\r\n");
		print_string(buffer+5);
		kprintf("\r\n");
		return 1;
	}
	return -1;
}

/***************** Main Function *****************/

void shell(struct buffers* interrupt_buffers){
	/* Initialize shell screen */
	clear_screen();
	kprintf("\nQuit with \"Ctrl-a x\"; or \"Ctrl-a c\" and then type in \"quit\".\n\n");

	/* Buffer to read the input command */
	int line_index = 0;							// Current position where we write
	int max_index = 0;							// Maximum length our line can have (does not have to be the real max, just above it)
	unsigned char* line_buffer;					// Buffer containing the command being typed
	reset_buffer(line_buffer);

	/* Some variables to help us later */
	unsigned char c;							// The character we are reading
	int flag;									// Flag returned by the function 'parse_line' (no use for now but might be for later)

	for (;;) {
		//while (0 == receive_char(interrupt_buffers -> rx,&c)){}
		if (1 == receive_char(interrupt_buffers -> rx,&c)){
			//kprinterr("Received char '%c' = '%x'\n\r",c,c);		// Sending the received character code to the second display for debugging purposes.
			switch (c){
				case '\r':
					// When we press enter
					if (-1 == (flag=parse_line(line_buffer))) {
						kprintf("\n\rDid not recognise command '");
						print_string(line_buffer);
						kprintf("', sorry!\r\n");
					}
					reset_buffer(line_buffer);
					line_index=0;
					break;
				case (char)0x7f:
					// Backspace delete
					line_index--;
					if(line_index<=0){
						line_index=0;
					}else{
						line_buffer[line_index] = '\0';
						shift_left(line_buffer,line_index);
					}
					break;
				case (char)0x1b:
					// Detecting a control character (del & arrow keys)
					escape_key(interrupt_buffers->rx,line_buffer,&line_index);
					break;
				default:
					// Should be a normal character
					insert_char(line_buffer,line_index,c);
					line_index++;
					break;
			}
			max_index = max_index>line_index?max_index:line_index;
			kputchar('\r');
			for(int i=0; i<max_index; i++) kputchar(' ');
			kputchar('\r');
			print_string(line_buffer);
			for(int i=0; i<len(line_buffer)-line_index; i++) kputchar('\b');
		}
		wfi();
	}
}