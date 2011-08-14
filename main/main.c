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

void msg_received(const void *data, const size_t data_len) {
    Packet *pkt = (Packet *)data;
    
    if (pkt->type == 'M') {
        // "meter"
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
    
    // decrease frequency of analog meter pwm; meter has capacitance that 
    // causes pwm=1 to show closer to 5v
    timer2_init(TIMER2_PRESCALE_256);
    timer2_config_wgm(TIMER2_WGM3); // fast PWM
    
    // configure PWM output for boost converter; OC0A
    timer0_config_oca(TIMER0_OCA2);

    // d/c == f_cpu/64 == f_cpu/1
    // 15% ==   25.2   ==    9.2
    // 25% ==   31.2   ==   12.4
    // 30% ==   32.8   ==   14.4
    // 40% ==   35.2   ==   16.4
    // 45% ==   35.6   ==   16
    // 50% ==   36.0   ==   16
    // 60% ==   36.0   ==   16.8
    // 75% ==   36     ==   28
    // 77% ==          ==   30.6
    // 80% ==          ==   35.2
    // 85% ==          ==   35.2
    // 90% ==   36     ==   35.4
    // 91% ==          ==   36
    // 95% ==          ==    0.4
    timer0_set_ocra((89*255)/100); // duty cycle
    
    // configure PWM output for LED; OC0B
    timer0_config_ocb(TIMER0_OCB2);
    timer0_set_ocrb(128); // duty cycle
    
    // configure PWM output for analog meter; OC2A
    timer2_config_oca(TIMER2_OCA2);
        
    // enable interrupts
    sei();
    
    // sweep the meter needle from 0 to 100%
    timer2_set_ocra(0);
    _delay_ms(2000);
    timer2_set_ocra(255);
    _delay_ms(2000);
    timer2_set_ocra(0);
    
    for (;;) {
        
    }
}

