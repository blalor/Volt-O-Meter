#include <avr/io.h>
#include <avr/interrupt.h>

#include "serial.h"
#include "serial_handler.h"
#include "timer0.h"
#include "timer2.h"

#include <util/delay.h>

#define RX_BUF_SIZE 2
static uint8_t rx_buf[RX_BUF_SIZE];

typedef struct __serial_pkt {
    char type;
    uint8_t value;
} Packet;

/*
 * Handles incoming messages from the serial handler
 *
 * @param data the received data
 * @param data_len the length of the received data
 */
void msg_received(const void *data, const size_t data_len) {
    Packet *pkt = (Packet *)data;
    
    if (pkt->type == 'M') {
        // "Meter"
        timer2_set_ocra(pkt->value);
        
        if (pkt->value == 0) {
            // disable PWM
            timer2_config_oca(TIMER2_OCA0);
        } else {
            // enable Fast PWM
            timer2_config_oca(TIMER2_OCA2);
        }
    } else if (pkt->type == 'B') {
        // "Boost"
        timer0_set_ocra(pkt->value);
    } else if (pkt->type == 'L') {
        // "LED"
        timer0_set_ocrb(pkt->value);
    }
}

int main(void) {
    // configure serial
    serial_handler_init(&msg_received, rx_buf, RX_BUF_SIZE, NULL);
    serial_init(BAUD_TO_UBRR(9600), &serial_handler_consume);

    // OC0A PD6  d6 pwm boost
    // OC0B PD5  d5 LED
    // OC1A PB1  d9 
    // OC1B PB2 d10 
    // OC2A PB3 d11 analog meter
    // OC2B PD3  d3 

    DDRD |= _BV(PD6) | _BV(PD5);
    DDRB |= _BV(PB3);
    
    // (future?) configure ADC input for boost converter feedback
    
    timer0_init(TIMER0_PRESCALE_1); // SRF of inductor is 0.15MHz
    timer0_config_wgm(TIMER0_WGM3); // fast PWM
    
    // configure PWM output for boost converter; OC0A
    timer0_config_oca(TIMER0_OCA2);
    
    // set duty cycle for boost converter; 89% gives about 34 volts
    timer0_set_ocra((89*255)/100); // duty cycle
    
    // configure PWM output for LED; OC0B
    timer0_config_ocb(TIMER0_OCB2);
    timer0_set_ocrb(128); // duty cycle
    
    // enable interrupts before setting up the meter, so that the boost 
    // converter's working
    sei();
    
    // sweep the meter needle from 0 to 100%
    PORTB |= _BV(PB3);
    _delay_ms(2000);
    PORTB &= ~_BV(PB3);
    _delay_ms(1000);
    
    // configure PWM output for analog meter; OC2A
    
    // decrease frequency of analog meter pwm; meter has capacitance that 
    // causes pwm=1 to show closer to 5v
    timer2_init(TIMER2_PRESCALE_256);
    timer2_config_wgm(TIMER2_WGM3); // fast PWM
    timer2_config_oca(TIMER2_OCA2);
        
    for (;;) {
        // @todo enable power saving
    }
}
