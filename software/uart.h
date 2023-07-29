#ifndef UART_H
#define	UART_H

#include <xc.h>

#ifdef	__cplusplus
extern "C" {
#endif

    #define RING_BUFFER_SIZE 9 // or 11
    extern volatile char ring_buffer[RING_BUFFER_SIZE];
    extern volatile uint8_t write_pointer;
    extern volatile uint8_t read_pointer;
    extern volatile __bit buffer_is_full;
    
    #define ROTATE_POINTER(pointer)             \
        (*pointer)++;                           \
        if (*pointer >= RING_BUFFER_SIZE) {     \
            *pointer = 0;                       \
        }                                       \
    
    /**
     * Initializes the UART periphery with Full-Dublex 8N1 at 9600 baud.
     */
    void init_uart(void);

    /**
     * To send a char over the tx-pin.
     * @param c char to send
     */
    inline void uart_send_char(char c);
    /**
     * To send a array of chars (string) over the tx-pin.
     * @param string to send
     */
    void uart_send_string(const char* string);
    
    /**
     * To check if a new character is in the internal FIFO available.
     * @return 1 if a new character is available
     */
    inline __bit uart_new_character_available(void);
    /**
     * If a new character is available, it returns the new character.
     * @return the new character
     */
    char uart_get_new_character(void);
    
#define WAIT_UNTIL_TX_COMPLETE() \
    while (TXSTAbits.TRMT == 0) { NOP(); }

#ifdef	__cplusplus
}
#endif

#endif	/* UART_H */

