#include "main.h"
#include "uart-irqs.h"
#include "uart.h"
#include "vic.h"
#include "cb.h"

/**
 * This is the C entry point, upcalled once the hardware has been setup properly
 * in assembly language, see the reset.s file.
 */
struct cb rxcb;
struct cb txcb;
struct buffers buff;


void uart_rx_handler(void* cookie){
	uint8_t c;
	uart_receive(UART0,&c);
	cb_put(((struct buffers*) cookie) -> rx,c);
}

void uart_rt_handler(void* cookie){
	uint8_t c;
	if (0 != uart_receive(UART0,&c)){
		cb_put(((struct buffers*) cookie) -> rx,c);
	}
}

void uart_init(struct buffers* buff){
	vic_setup();
	
	uint32_t lcr = *(uint32_t*) (UART0 + CUARTLCR_H);
	lcr |= CUARTLCR_H_FEN;
	*(uint16_t *) (UART0 + CUARTLCR_H) = lcr;

	vic_irq_enable(UART0_IRQ,uart_rx_handler,(void*)buff);
	//( ,uart_rt_handler,(void*)buff);
	
	unsigned short* imsc = (unsigned short*) (UART0 + UART_IMSC);
	*imsc = *imsc | UART_IMSC_RXIM;//| UART_IMSC_RTIM;
	
	vic_enable();
}

void _start() {
	//uart_send_string(UART0,"Starting\n\r");
	cb_init(&rxcb);
	cb_init(&txcb);
	buff.rx = &rxcb;
	buff.tx = &txcb;

	uart_init(&buff);

	shell(&buff);
}
