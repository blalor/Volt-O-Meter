#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>

#define BAUD_TO_UBRR(baud_rate) (F_CPU/16/baud_rate)-1

void serial_init(const uint16_t baud_rate, void (*tx_byte_fn)(const uint8_t));

#endif
