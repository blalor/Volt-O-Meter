#ifndef TIMER0_H
#define TIMER0_H

#include <avr/io.h>

typedef enum __timer0_prescalers {
    TIMER0_PRESCALE_OFF  = ~(_BV(CS02) | _BV(CS01) | _BV(CS00)),
    TIMER0_PRESCALE_1    =                           _BV(CS00) ,
    TIMER0_PRESCALE_8    =               _BV(CS01)             ,
    TIMER0_PRESCALE_64   =               _BV(CS01) | _BV(CS00) ,
    TIMER0_PRESCALE_256  =   _BV(CS02)                         ,
    TIMER0_PRESCALE_1024 =   _BV(CS02) |             _BV(CS00) ,
} Timer0Prescale;

typedef enum __timer0_wgm_modes {
    TIMER0_WGM3,
} Timer0WaveGenModes;

// "modes" map like waveform generation modes
//     00 == 0, 01 == 1, 10 == 2, 11 == 3
typedef enum __timer0_oca_modes {
    TIMER0_OCA0 = 0 /* ~(_BV(COM0A1) | _BV(COM0A0)) */,
    // TIMER0_OCA1 =                 _BV(COM0A0),
    TIMER0_OCA2 =   _BV(COM0A1),
    // TIMER0_OCA3 =   _BV(COM0A1) | _BV(COM0A0),
} Timer0OutputCompareAModes;

typedef enum __timer0_ocb_modes {
    TIMER0_OCB0 = 0 /* ~(_BV(COM0B1) | _BV(COM0B0)) */,
    // "mode 1" is reserved
    TIMER0_OCB2 =   _BV(COM0B1),
    // TIMER0_OCB3 =   _BV(COM0B1) | _BV(COM0B0),
} Timer0OutputCompareBModes;

void timer0_init(const Timer0Prescale prescale);

// used together, these set the PWM mode and the duty cycle for each PWM channel
void timer0_config_wgm(const Timer0WaveGenModes wgm);
void timer0_config_oca(const Timer0OutputCompareAModes oca_mode);
void timer0_config_ocb(const Timer0OutputCompareBModes ocb_mode);

void timer0_set_ocra(const uint8_t ocra);
void timer0_set_ocrb(const uint8_t ocrb);

#endif
