#include "serial.h"

#include <avr/io.h>
#include <avr/interrupt.h>

static void (*rx_byte_handler)(const uint8_t);

void serial_init(
    const uint16_t baud_rate,
    void (*tx_byte_fn)(const uint8_t)
) {
    rx_byte_handler = tx_byte_fn;
    
    UBRR0 = baud_rate;
    
    UCSR0A = 0; // clear flags
    
    // enable receiver and RX Complete Interrupt
    UCSR0B = _BV(RXCIE0) | _BV(RXEN0);
}

ISR(USART_RX_vect) {
    rx_byte_handler(UDR0);
}
