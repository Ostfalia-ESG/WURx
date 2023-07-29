#include <stdio.h>
#include <stdlib.h>
#include <xc.h>

#include "main.h"
#include "uart.h"
#include "configuration.h"

/**
 * ring buffer for the uart receiving
 */
volatile char ring_buffer[RING_BUFFER_SIZE];
volatile uint8_t write_pointer = 0;
volatile uint8_t read_pointer = 0;
volatile __bit buffer_is_full = 0;

void init_uart(void) {
    /**
     * Baud rate is when the configuration is changed (or due loading).
     * See configuration.c
     */
       
    // set asynchronous mode
    TXSTAbits.SYNC = 0;
    // enable serial port (configures rx/tx pin)
    RCSTAbits.SPEN = 1;
    // enable the transmitter
    TXSTAbits.TXEN = 1;
    // enable the receiver (continuous)
    RCSTAbits.CREN = 1;
}


void uart_send_char(char c) {
    // waiting until the transmit shift register is empty
    WAIT_UNTIL_TX_COMPLETE();
    // transmit the char
    TXREGbits.TXREG = c;
}

// commented out to remove warnings, for unused function
//void uart_send_string(const char* string) {
//    int idx = 0;
//    while(string[idx] != '\0') {
//        uart_send_char(string[idx]);
//        idx++;
//    }
//}


__bit uart_new_character_available(void) {
    return write_pointer != read_pointer || buffer_is_full;
}

char uart_get_new_character(void) {
    char new_char = ring_buffer[read_pointer];
    ROTATE_POINTER(&read_pointer);
    buffer_is_full = 0;
    return new_char;
}