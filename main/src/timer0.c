#include "timer0.h"

void timer0_init(const Timer0Prescale prescale) {
    TCCR0A = 0;
    TCCR0B |= prescale;
    
    // clear flags
    TIFR0 = 0;
}

void timer0_config_wgm(const Timer0WaveGenModes wgm) {
    switch (wgm) {
        case TIMER0_WGM3:
            TCCR0A |= _BV(WGM01) | _BV(WGM00);
            TCCR0B &= ~_BV(WGM02);
            break;
        
        default:
            // no default
            break;
    }
}

void timer0_config_oca(const Timer0OutputCompareAModes oca_mode) {
    // clear bits occupied by COM0A1 and COM0A0, leaving other bits intact
    TCCR0A = (
        TCCR0A & (
            ~(_BV(COM0A1) | _BV(COM0A0))
        )
    ) | oca_mode;
}

void timer0_config_ocb(const Timer0OutputCompareBModes ocb_mode) {
    // clear bits occupied by COM0B1 and COM0B0, leaving other bits intact
    TCCR0A = (
        TCCR0A & (
            ~(_BV(COM0B1) | _BV(COM0B0))
        )
    ) | ocb_mode;
}

inline void timer0_set_ocra(const uint8_t ocra) {
    OCR0A = ocra;
}

inline void timer0_set_ocrb(const uint8_t ocrb) {
    OCR0B = ocrb;
}

