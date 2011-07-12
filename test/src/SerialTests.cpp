#include "CppUTest/TestHarness.h"
#include "8bit_binary.h"
#include <stdint.h>

extern "C" {
    #include <avr/io.h>
    
    #include "serial.h"
    
    #include "ByteReceiverSpy.h"
    
    void ISR_USART_RX_vect(void);
}

const uint32_t baud_rate = 9600;

TEST_GROUP(SerialTests) {
    void setup() {
        brs_init();
        
        virtualUCSR0A = 0xff;
        virtualUCSR0B = 0;
        virtualUBRR0 = 0;
        
        serial_init(BAUD_TO_UBRR(baud_rate), &brs_receive_byte);
    }
    
    void teardown() {
        
    }
};

/*
 * confirm proper setup after initialization
 */
TEST(SerialTests, Initialization) {
    LONGS_EQUAL(51, virtualUBRR0); // baud rate register
    BYTES_EQUAL(0, virtualUCSR0A); // clear all flags
    BYTES_EQUAL(B10010000, virtualUCSR0B); // UART configured for RX, with interrupts
}

TEST(SerialTests, ByteReceived) {
    virtualUDR0 = 'a';
    
    ISR_USART_RX_vect();
    
    BYTES_EQUAL(1, brs_get_invocation_count());
    BYTES_EQUAL('a', brs_get_received_byte());
}
