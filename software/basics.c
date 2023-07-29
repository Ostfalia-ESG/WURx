#include <stdio.h>
#include <stdlib.h>
#include <xc.h>

#include "basics.h"
#include "interrupt.h"
#include "main.h"

void do_deep_sleep(void) {
    SLEEP();
    NOP(); // needed after SLEEP command (next operation is prefetched)
}

void init_gpio_rf_ir(void) {
    // set pin A5 as digital I/O
    // ANSELAbits.ANSA5 = 0; (is default))
    // set pin A5 as input
    TRISAbits.TRISA5 = 1;
    // enable weak pull-up
    WPUAbits.WPUA5 = 1;
}


void init_gpio_rb_ir_conf_en(void) {
    // set pin A2 as digital I/O
    ANSELAbits.ANSA2 = 0;
    // set pin A2 as input (for conf_en)
    TRISAbits.TRISA2 = 1;
    // disable weak pull-up (use external pull-up)
    WPUAbits.WPUA2 = 0;
    
    // enable interrupt
    ENABLE_INTERRUPT_RB_IR_CONF_EN(1);
}


void init_gpio_rf_data(void) {
    // set pin A4 as digital I/O
    ANSELAbits.ANSA4 = 0;
    // set pin A4 as input
    TRISAbits.TRISA4 = 1;
    // enable weak pull-up
    WPUAbits.WPUA4 = 1;
}

void init_gpio_rf_discharge(void) {
    // set pin C1 as digital I/O
    ANSELCbits.ANSC1 = 0;
    // set pin C1 as output
    TRISCbits.TRISC1 = 0;
    // disable weak pull-up
    WPUCbits.WPUC1 = 0;
    // default is low
    LATCbits.LATC1 = 0;
}

 void init_gpio_rf_ir_hold(void) {
    // set pin C0 as digital I/O
    ANSELCbits.ANSC0 = 0;
    // set pin C0 as output
    TRISCbits.TRISC0 = 0;
    // disable weak pull-up
    WPUCbits.WPUC0 = 0;
#ifdef ENABLE_TOTT
    // default is high (self hold)
    LATCbits.LATC0 = 1;
#else
    LATCbits.LATC0 = 0;
#endif
 }
 
void init_gpio_debug_sampling(void) {
    // set pin C2 as digital I/O
    ANSELCbits.ANSC2 = 0;
    // set pin C2 as output
    TRISCbits.TRISC2 = 0;
    // disable weak pull-up
    WPUCbits.WPUC2 = 0;
}