#ifndef SERIAL_HANDLER_H
#define SERIAL_HANDLER_H

/*
Processes and (eventually) generates messages of the form
    0xff 0x55 <length> <data1> … <dataN> <checksum>

This is modeled on (cloned from) the iPod serial protocol.

There are deficiencies in the protocol: see sad-path tests.
*/

#include <stdint.h> // uint8_t
#include <stddef.h> // size_t

/*
 * initializes serial protocol handler.
 *
 * @param received_msg_handler callback invoked when valid message received
 * @param rx_buf buffer for storing data being received
 * @param rx_buf_len size of rx_buf
 * @param tx_byte_fn function used to transmit a byte
 */
void serial_handler_init(
    void (*received_msg_handler)(const void *data, const size_t data_len),
    uint8_t *rx_buf,
    uint8_t rx_buf_len,
    uint8_t (*tx_byte_fn)(const uint8_t byte)
);

/*
 * consumes a byte. passed to serial driver as a callback.
 * invoked in the ISR; must be fast!
 *
 * @param byte the byte received by the serial driver
 */
void serial_handler_consume(const uint8_t byte);

/*
 * sends data with the header, length, and checksum.
 *
 * @param data the data to send
 * @param data_len the length of the data
 */
void serial_handler_send(const void *data, const size_t data_len);
#endif
