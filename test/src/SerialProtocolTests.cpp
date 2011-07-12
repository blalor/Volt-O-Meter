#include "CppUTest/TestHarness.h"
#include "8bit_binary.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

extern "C" {
    #include "serial_handler.h"
}

typedef struct __dummy_struct {
    uint8_t a;
    uint16_t b;
} DummyStruct;

static void *spy_msg;

#define RX_DATA_BUF_SIZE 50
static uint8_t rx_data_buf[RX_DATA_BUF_SIZE];

#define TX_DATA_BUF_SIZE 50
static uint8_t tx_data_buf[TX_DATA_BUF_SIZE];
static uint8_t tx_data_buf_ind;

void spy_receive_msg(const void *msg, const size_t len) {
    free(spy_msg);
    spy_msg = malloc(len + 5);
    memcpy(spy_msg, msg, len);
}

uint8_t spy_tx_byte(const uint8_t b) {
    tx_data_buf[tx_data_buf_ind++] = b;
    
    return 0;
}

TEST_GROUP(SerialProtocolTests) {
    void setup() {
        spy_msg = (uint8_t *)malloc(1);
        memset(spy_msg, 0, 1);

        memset(tx_data_buf, 0, TX_DATA_BUF_SIZE);
        tx_data_buf_ind = 0;
        
        serial_handler_init(
            spy_receive_msg, rx_data_buf, RX_DATA_BUF_SIZE,
            spy_tx_byte
        );
    }
    
    void teardown() {
        free(spy_msg);
        spy_msg = NULL;
    }
    
    void send_message(const char *msg_data, const uint8_t msg_len) {
        for (uint8_t i = 0; i < msg_len; i++) {
            serial_handler_consume(msg_data[i]);
        }
    }
};

/*
The following Python snippet will generate the checksum for `data'

chksum = len(data)
for b in data:
    chksum += ord(b)

chksum = (0x100 - (chksum & 0xFF))
*/

TEST(SerialProtocolTests, ParseMessage) {
    send_message("\xff\x55\x04test\x3c", 8);
    
    STRCMP_EQUAL("test", (const char *)spy_msg);
}

TEST(SerialProtocolTests, ParseMessageInvalidChecksum) {
    free(spy_msg);
    spy_msg = NULL;
    
    send_message("\xff\x55\x04testc", 8);
    
    POINTERS_EQUAL(NULL, spy_msg);
}

TEST(SerialProtocolTests, ParseMessageEmbeddedStartMarker) {
    send_message("\xff\x55\x06te\xff\x55st\xe6", 10);

    STRCMP_EQUAL("te\xff\x55st", (const char *)spy_msg);
}

// more tests needed to test recovery from bad message
TEST(SerialProtocolTests, RecoveryFromLongMessage) {
    // send message with invalid data length and checksum
    send_message("\xff\x55\x06waytoolong\xe6", 14);
    
    // valid message
    // ow, my brain!
    // "\xff\x55\x07f" is being parsed as 0xff 0x55 0x7f! gcc bug?
    send_message(
        (const char[]){0xff, 0x55, 0x07, 'f', 'o', 'o', ' ', 'b', 'a', 'r', 0x60},
        11
    );
    
    STRCMP_EQUAL("foo bar", (const char *)spy_msg);
}

TEST(SerialProtocolTests, RecoveryFromShortMessage) {
    // send message with invalid data length and checksum
    // 0x14 == 20, data is only 10 bytes long (11 w/ checksum); so header and
    // data length of next message will be swallowed and lost
    // NOTE: I don't like this; feels too clunky. Maybe add a scheduler task,
    // if the protocol can't be improved upon?
    send_message("\xff\x55\x14waytoolong\xe6", 14);
    
    // subsequent message will be lost
    send_message("\xff\x55\x08testtest\x78", 12);
    
    // valid message
    // ow, my brain!
    // "\xff\x55\x07f" is being parsed as 0xff 0x55 0x7f! gcc bug?
    send_message(
        (const char[]){0xff, 0x55, 0x07, 'f', 'o', 'o', ' ', 'b', 'a', 'r', 0x60},
        11
    );
    
    STRCMP_EQUAL("foo bar", (const char *)spy_msg);
}

TEST(SerialProtocolTests, SendMessageFromStruct) {
    const DummyStruct dummy = {42, 4200};
    
    // CHECK_TRUE(sizeof(uint8_t) == sizeof(size_t));
    
    // for (uint8_t i = 0; i < sizeof(dummy); i++) {
    //     printf("dummy[%d] = %02x\n", i, ((uint8_t *)&dummy)[i]);
    // }
    
    // dummy[0] == 0x2a
    // dummy[1] == 0x00
    // dummy[2] == 0x68
    // dummy[3] == 0x10
    
    uint8_t *compare_buf = (uint8_t *)malloc(sizeof(dummy) + 4);
    compare_buf[0] = 0xff;
    compare_buf[1] = 0x55;
    compare_buf[2] = sizeof(dummy);
    memcpy(&compare_buf[3], &dummy, sizeof(dummy));
    compare_buf[7] = 0x5a; // checksum

    serial_handler_send(&dummy, sizeof(dummy));
    
    // for (uint8_t i = 0; i < sizeof(dummy) + 4; i++) {
    //     printf("compare_buf[%d] = %02x\n", i, compare_buf[i]);
    // }
    
    // compare_buf[0] == 0xff
    // compare_buf[1] == 0x55
    // compare_buf[2] == 0x04
    // compare_buf[3] == 0x2a
    // compare_buf[4] == 0x00
    // compare_buf[5] == 0x68
    // compare_buf[6] == 0x10
    // compare_buf[7] == 0x5a
    
    BYTES_EQUAL(compare_buf[0], tx_data_buf[0]);
    BYTES_EQUAL(compare_buf[1], tx_data_buf[1]);
    BYTES_EQUAL(compare_buf[2], tx_data_buf[2]);
    BYTES_EQUAL(compare_buf[3], tx_data_buf[3]);
    BYTES_EQUAL(compare_buf[4], tx_data_buf[4]);
    BYTES_EQUAL(compare_buf[5], tx_data_buf[5]);
    BYTES_EQUAL(compare_buf[6], tx_data_buf[6]);
    BYTES_EQUAL(compare_buf[7], tx_data_buf[7]);
    
    free(compare_buf);
    compare_buf = NULL;
}

TEST(SerialProtocolTests, SendMessageFromString) {
    const char *data_to_send = "this is a test";
    
    char compare_buf[] = {
        0xff, 0x55, 14,
        't', 'h', 'i', 's', ' ', 'i', 's', ' ', 'a', ' ', 't', 'e', 's', 't',
        0xdd, // checksum
    };

    serial_handler_send(data_to_send, strlen(data_to_send));

    STRCMP_EQUAL(compare_buf, (char *)tx_data_buf);
}
