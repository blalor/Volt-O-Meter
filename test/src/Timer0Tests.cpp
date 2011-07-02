#include "CppUTest/TestHarness.h"
#include "8bit_binary.h"

#include <stdint.h>

extern "C" {
    #include <avr/io.h>
    
    #include "timer0.h"
}

TEST_GROUP(Timer0Tests) {
    void setup() {
        virtualTCCR0A = 0xff;
        virtualTCCR0B = 0;
        virtualTIFR0 = 0xff;
        
        timer0_init(TIMER0_PRESCALE_1);
    }
    
    void teardown() {
    }
};

/*
 * confirm proper setup after initialization
 */
TEST(Timer0Tests, Initialization) {
    BYTES_EQUAL(B00000000, virtualTCCR0A);
    BYTES_EQUAL(B00000001, virtualTCCR0B);
    BYTES_EQUAL(B00000000, virtualTIFR0);
}

TEST(Timer0Tests, ConfigWGM) {
    virtualTCCR0A = 0;
    virtualTCCR0B = 0xff;
    
    timer0_config_wgm(TIMER0_WGM3);
    
    BYTES_EQUAL(B00000011, virtualTCCR0A);
    BYTES_EQUAL(B11110111, virtualTCCR0B);
}

TEST(Timer0Tests, SetOC0A_Mode0) {
    virtualTCCR0A = B11100010;
    
    // configure Output Compare A
    timer0_config_oca(TIMER0_OCA0);
    
    // ensure other bits aren't set or cleared
    BYTES_EQUAL(B00100010, virtualTCCR0A);
}

TEST(Timer0Tests, SetOC0A_Mode2) {
    virtualTCCR0A = B01000000;
    
    // configure Output Compare A
    timer0_config_oca(TIMER0_OCA2);
    
    BYTES_EQUAL(B10000000, virtualTCCR0A);
}

TEST(Timer0Tests, SetOC0B_Mode0) {
    // bits 5, 4
    virtualTCCR0A = B01110010;
    
    // configure Output Compare B
    timer0_config_ocb(TIMER0_OCB0);
    
    // ensure other bits aren't set or cleared
    BYTES_EQUAL(B01000010, virtualTCCR0A);
}

TEST(Timer0Tests, SetOC0B_Mode2) {
    virtualTCCR0A = B10010001;
    
    // configure Output Compare B
    timer0_config_ocb(TIMER0_OCB2);
    
    BYTES_EQUAL(B10100001, virtualTCCR0A);
}

TEST(Timer0Tests, SetOCR0A) {
    virtualOCR0A = 99;
    
    timer0_set_ocra(42);
    
    BYTES_EQUAL(42, virtualOCR0A);
}

TEST(Timer0Tests, SetOCR0B) {
    virtualOCR0B = 99;
    
    timer0_set_ocrb(42);
    
    BYTES_EQUAL(42, virtualOCR0B);
}
