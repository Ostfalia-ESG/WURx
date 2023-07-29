#include <stdio.h>
#include "uart.h"

/**
 * Remap putch() function for using printf()
 * Attention: the printf() implementation needs much data space. 
 * Therfore it couldn't be used in this project.
 * @param c char to send over uart
 */
//void putch(char c)
//{
//    uart_send_char(c);
//}
