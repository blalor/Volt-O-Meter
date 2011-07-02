#ifndef TIMER2_H
#define TIMER2_H

#include <avr/io.h>

typedef enum __timer2_prescalers {
    TIMER2_PRESCALE_OFF  = ~(_BV(CS22) | _BV(CS21) | _BV(CS20)),
    TIMER2_PRESCALE_1    =                           _BV(CS20) ,
    TIMER2_PRESCALE_8    =               _BV(CS21)             ,
    TIMER2_PRESCALE_64   =               _BV(CS21) | _BV(CS20) ,
    TIMER2_PRESCALE_256  =   _BV(CS22)                         ,
    TIMER2_PRESCALE_1024 =   _BV(CS22) |             _BV(CS20) ,
} Timer2Prescale;

typedef enum __timer2_wgm_modes {
    TIMER2_WGM3,
} Timer2WaveGenModes;

// "modes" map like waveform generation modes
//     00 == 0, 01 == 1, 10 == 2, 11 == 3
typedef enum __timer2_oca_modes {
    TIMER2_OCA0 = 0 /* ~(_BV(COM2A1) | _BV(COM2A0)) */,
    // TIMER2_OCA1 =                 _BV(COM2A0),
    TIMER2_OCA2 =   _BV(COM2A1),
    // TIMER2_OCA3 =   _BV(COM2A1) | _BV(COM2A0),
} Timer2OutputCompareAModes;

typedef enum __timer2_ocb_modes {
    TIMER2_OCB0 = 0 /* ~(_BV(COM2B1) | _BV(COM2B0)) */,
    // "mode 1" is reserved
    TIMER2_OCB2 =   _BV(COM2B1),
    // TIMER0_OCB3 =   _BV(COM2B1) | _BV(COM2B0),
} Timer2OutputCompareBModes;

void timer2_init(const Timer2Prescale prescale);

// used together, these set the PWM mode and the duty cycle for each PWM channel
void timer2_config_wgm(const Timer2WaveGenModes wgm);
void timer2_config_oca(const Timer2OutputCompareAModes oca_mode);
void timer2_config_ocb(const Timer2OutputCompareBModes ocb_mode);

void timer2_set_ocra(const uint8_t ocra);
void timer2_set_ocrb(const uint8_t ocrb);

#endif
