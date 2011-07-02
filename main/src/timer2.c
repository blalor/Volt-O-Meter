#include "timer2.h"

void timer2_init(const Timer2Prescale prescale) {
    TCCR2A = 0;
    TCCR2B |= prescale;
    
    // clear flags
    TIFR2 = 0;
}

void timer2_config_wgm(const Timer2WaveGenModes wgm) {
    switch (wgm) {
        case TIMER2_WGM3:
            TCCR2A |= _BV(WGM21) | _BV(WGM20);
            TCCR2B &= ~_BV(WGM22);
            break;
        
        default:
            // no default
            break;
    }
}

void timer2_config_oca(const Timer2OutputCompareAModes oca_mode) {
    // clear bits occupied by COM0A1 and COM0A0, leaving other bits intact
    TCCR2A = (
        TCCR2A & (
            ~(_BV(COM2A1) | _BV(COM2A0))
        )
    ) | oca_mode;
}

void timer2_config_ocb(const Timer2OutputCompareBModes ocb_mode) {
    // clear bits occupied by COM0B1 and COM0B0, leaving other bits intact
    TCCR2A = (
        TCCR2A & (
            ~(_BV(COM2B1) | _BV(COM2B0))
        )
    ) | ocb_mode;
}

inline void timer2_set_ocra(const uint8_t ocra) {
    OCR2A = ocra;
}

inline void timer2_set_ocrb(const uint8_t ocrb) {
    OCR2B = ocrb;
}

