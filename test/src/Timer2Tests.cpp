#include "CppUTest/TestHarness.h"
#include "8bit_binary.h"

#include <stdint.h>

extern "C" {
    #include <avr/io.h>
    
    #include "timer2.h"
}

TEST_GROUP(Timer2Tests) {
    void setup() {
        virtualTCCR2A = 0xff;
        virtualTCCR2B = 0;
        virtualTIFR2 = 0xff;
        
        timer2_init(TIMER2_PRESCALE_1);
    }
    
    void teardown() {
    }
};

/*
 * confirm proper setup after initialization
 */
TEST(Timer2Tests, Initialization) {
    BYTES_EQUAL(B00000000, virtualTCCR2A);
    BYTES_EQUAL(B00000001, virtualTCCR2B);
    BYTES_EQUAL(B00000000, virtualTIFR2);
}

TEST(Timer2Tests, ConfigWGM) {
    virtualTCCR2A = 0;
    virtualTCCR2B = 0xff;
    
    timer2_config_wgm(TIMER2_WGM3);
    
    BYTES_EQUAL(B00000011, virtualTCCR2A);
    BYTES_EQUAL(B11110111, virtualTCCR2B);
}

TEST(Timer2Tests, SetOC2A_Mode0) {
    virtualTCCR2A = B11100010;
    
    // configure Output Compare A
    timer2_config_oca(TIMER2_OCA0);
    
    // ensure other bits aren't set or cleared
    BYTES_EQUAL(B00100010, virtualTCCR2A);
}

TEST(Timer2Tests, SetOC2A_Mode2) {
    virtualTCCR2A = B01000000;
    
    // configure Output Compare A
    timer2_config_oca(TIMER2_OCA2);
    
    BYTES_EQUAL(B10000000, virtualTCCR2A);
}

TEST(Timer2Tests, SetOC2B_Mode0) {
    // bits 5, 4
    virtualTCCR2A = B01110010;
    
    // configure Output Compare B
    timer2_config_ocb(TIMER2_OCB0);
    
    // ensure other bits aren't set or cleared
    BYTES_EQUAL(B01000010, virtualTCCR2A);
}

TEST(Timer2Tests, SetOC2B_Mode2) {
    virtualTCCR2A = B10010001;
    
    // configure Output Compare B
    timer2_config_ocb(TIMER2_OCB2);
    
    BYTES_EQUAL(B10100001, virtualTCCR2A);
}

TEST(Timer2Tests, SetOCR2A) {
    virtualOCR2A = 99;
    
    timer2_set_ocra(42);
    
    BYTES_EQUAL(42, virtualOCR2A);
}

TEST(Timer2Tests, SetOCR2B) {
    virtualOCR2B = 99;
    
    timer2_set_ocrb(42);
    
    BYTES_EQUAL(42, virtualOCR2B);
}

