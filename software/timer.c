#include <stdio.h>
#include <stdlib.h>
#include <xc.h>

#include "main.h"
#include "timer.h"
#include "configuration.h"

void init_timer_sampling(void) {
    // first disable timer & clear counter
    T1CONbits.TMR1ON = 0;
    TMR1Hbits.TMR1H = 0x00;
    TMR1Lbits.TMR1L = 0x00;
    
    // timer 1 counts regardless of gate function
    T1GCONbits.TMR1GE = 0;
    // select system clock Fosc/4
    // important: timer Fosc (01) is not working with the CPP1 module
    T1CONbits.TMR1CS = 00;
    // select prescaler 1:1
    T1CONbits.T1CKPS = 00;
    
    /*
     * init compare mode
     */
    // compare mode: Special Event Trigger (resets timer and interrupt)
    CCP1CONbits.CCP1M = 0b1011; 
}

void init_timer_discharge(void) {
    // set 1:1 prescaler
    T2CONbits.T2CKPS = 0b00;
    // set 1:1 postscaler
    T2CONbits.T2OUTPS = 0b0000;
}
