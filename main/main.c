#include <avr/io.h>
#include <avr/interrupt.h>

#include "serial.h"
#include "timer0.h"
#include "timer2.h"

void byte_received(const uint8_t the_byte) {
    
}

int main(void) {
    // configure serial
    serial_init(BAUD_TO_UBRR(9600), &byte_received);
    
    // (future?) configure ADC input for boost converter feedback
    
    timer0_init(TIMER0_PRESCALE_1);
    timer0_config_wgm(TIMER0_WGM3); // fast PWM
    
    timer2_init(TIMER2_PRESCALE_1);
    timer2_config_wgm(TIMER2_WGM3); // fast PWM
    
    // configure PWM output for boost converter
    timer0_config_oca(TIMER0_OCA2);
    timer0_set_ocra(128); // duty cycle
    
    // configure PWM output for analog meter
    timer0_config_ocb(TIMER0_OCB2);
    timer0_set_ocrb(128); // duty cycle
    
    // configure PWM output for illumination LED
    timer2_config_oca(TIMER2_OCA2);
    timer2_set_ocra(128); // duty cycle
    
    sei();
    
    for (;;) {
        
    }
}

