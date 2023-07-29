#include <stdio.h>
#include <stdlib.h>
#include <xc.h>

#include "interrupt.h"

/**
 * https://microchipdeveloper.com/8bit:interrupts
 */

void init_interrupts(void) {
    // enable all active interrupts
    INTCONbits.GIE = 1;
    // enable all peripheral interrupts
    INTCONbits.PEIE = 1;
    
    /**
     * for the ir_input_pin
     */
    // enable interrupt-on-change
    INTCONbits.IOCIE = 1;
    // enable flags for interrupt-on-change
    INTCONbits.IOCIF = 1;
    
    /**
     * for the timer 1
     */
    // enable interrupt for compare module
    PIE1bits.CCP1IE = 1;
    
    /**
     * for the timer 2 (discharge)
     */
    // enable interrupt
    PIE1bits.TMR2IE = 1;
    
    /**
     * for uart peripheral
     */
    // enable USART receive interrupt
    PIE1bits.RCIE = 1;
}

