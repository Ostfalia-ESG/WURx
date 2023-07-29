// CONFIG1
#pragma config FOSC = INTOSC    // Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Memory Code Protection (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable (Brown-out Reset disabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = OFF       // Internal/External Switchover (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is disabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config PLLEN = OFF      // PLL Enable (4x PLL disabled)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LVP = OFF         // Low-Voltage Programming Enable (Low-voltage programming enabled)


#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <pic16lf1823.h>


#include "main.h"
#include "basics.h"
#include "timer.h"
#include "interrupt.h"
#include "uart.h"
#include "configuration.h"

volatile __bit ongoing_address_sampling = 0;

inline void confirm_wake_up_call(void);
inline void clear_to_sleep(void);

/*
 * Entrypoint of the application.
 * It initializes all peripheries and then switches to sleep mode 
 * until a start bit occurs.
 */
int main(void) {
#ifndef WUR_COMPILE_RELEASE
#warning No release compilation: leads to slow sampling.
#endif
    // initilize with 16 MHz to speed up the initialization routines
    OSCCONbits.IRCF = 0b1111;
    // self holding first (only important if ENABLE_TOTT is active)
    init_gpio_rf_ir_hold();
 
#ifdef SHOW_SAMPLING_POINTS
    // signals the start of the initilization routines
    init_gpio_debug_sampling();
    SET_GPIO_DEBUG_SAMPLING(1);
    NOP();
    NOP();
    SET_GPIO_DEBUG_SAMPLING(0);
#endif

    // load and init configuration (Address, Bitrate) from EEPROM 
    read_config_from_eeprom();
    init_wur_configuration(0);
    
    init_interrupts();
    init_gpio_rf_ir();
    init_gpio_rf_data();
    init_gpio_rb_ir_conf_en();
    init_gpio_rf_discharge();
    init_timer_sampling();
    init_timer_discharge();
    init_uart();
    
#ifdef SHOW_SAMPLING_POINTS 
    // signals the end of the initilization routines
    SET_GPIO_DEBUG_SAMPLING(1);
    NOP();
    NOP();
    SET_GPIO_DEBUG_SAMPLING(0);
#endif

    // user lower speed after initilization to reduce power
    OSCCONbits.IRCF = config_osccon_bits;
    
#ifndef ENABLE_TOTT
    // sleep after start and wait for interrupt on rf_ir
    ENABLE_INTERRUPT_RF_IR(1);
#else
    // When TOTT is enabled, address decoding starts directly after uC initialization
    ongoing_address_sampling = 1;
    if (wur_config.address_length != 0) {
        // wait for the sync bit
        ENABLE_INTERRUPT_RF_DATA(1); 
    } else {
        // zero address mode, confirm WUC
        confirm_wake_up_call();
        clear_to_sleep();
    }
#endif
    
    while(1) {
        if (ongoing_address_sampling == 0 && PORTAbits.RA2 == 1) {
            // if nothing is todo, go to sleep
            do_deep_sleep();
        }
        if (ongoing_address_sampling == 1) {
            // stay active during ongoing address sampling
            NOP();
        }
        // update config wehn somethin was sent via UART
        conf_loop();
    }
    return (EXIT_SUCCESS);
}

/**
 * Confirms the recept of a valid WUC for the backend.
 */
inline void confirm_wake_up_call(void) {
    SET_GPIO_RB_IR_CONF_EN();
    NOP();
    NOP();
    RESET_GPIO_RB_IR_CONF_EN();
}

/**
 * Discharges the frontend (clean state) and then goes to sleep.
 */
inline void clear_to_sleep(void) {
    // discharge frontend capacity (C6)
    while(READ_RF_DATA() == 1) {
        // wait until data pin is low
        // protects against energy waste
    }
    
    // discharge frontend for some time
    SET_GPIO_RF_DISCHARGE(1);
    START_TIMER_DISCHARGING();
}


// keeps the currently received part of the address
uint8_t address_pos = 0;
uint32_t address;

/**
 * The one interrupt service routine, that handles all events.
 */
void __interrupt() ISR_wur(void) {
    // To get the best performance, the timer 1 flag is checked first.
    if (PIR1bits.CCP1IF == 1) { // sample the current bit from data pin
        // sample current bit on data pin first!
        uint8_t data_bit = READ_RF_DATA();
        
#ifdef SHOW_SAMPLING_POINTS
        // to display sample point over pin C2
        SET_GPIO_DEBUG_SAMPLING(1);
        NOP();
        NOP();
        SET_GPIO_DEBUG_SAMPLING(0);
#endif
        
        // clear flag
        PIR1bits.CCP1IF = 0;
        
        // deocde address (MSB first)
        address_pos--;
        address <<= 1;
        address |= data_bit;
        
        if (address_pos == 0) { // all address bits sampled
            // disable sampling timer
            DISABLE_TIMER_SAMPLING();
            
            // validate the received address
            if (address == wur_config.address) {
                // correct address received
                confirm_wake_up_call();
            }
            
            address = 0;    // clear address
            clear_to_sleep();
        }
    } else if (IOCAFbits.IOCAF4 == 1) { // sync bit received
        // clear status flag
        IOCAFbits.IOCAF4 = 0;
        // disable interrupt on data pin
        ENABLE_INTERRUPT_RF_DATA(0);
        
        // start sampling center wise
        TIMER_START_VALUE(config_target_sync_timer_value);
        ENABLE_TIMER_SAMPLING();
        while(PIR1bits.CCP1IF == 0) { /** wait half a baud period **/ }
        // clear flag
        PIR1bits.CCP1IF = 0;
        
#ifdef SHOW_SAMPLING_POINTS
        // to display sync point over pin C2
        SET_GPIO_DEBUG_SAMPLING(1);
        NOP();
        SET_GPIO_DEBUG_SAMPLING(0);
#endif
        
        // set address starting position (MSB first)
        address_pos = wur_config.address_length; 
    } else if (IOCAFbits.IOCAF5 == 1) { // interrupt on rf_ir (start of a WUC)
        // clear status flag
        IOCAFbits.IOCAF5 = 0;
        
        if (wur_config.address_length != 0) { // start address decoding
            // disable interrupt
            ENABLE_INTERRUPT_RF_IR(0);
            // wait for the sync bit
            ENABLE_INTERRUPT_RF_DATA(1); 
            ongoing_address_sampling = 1;
        } else { // zero address mode, confirm WUC
            confirm_wake_up_call();
            clear_to_sleep();
        }
        
    } else if (IOCAFbits.IOCAF2 == 1) { // config mode enabled
        // clear status flag
        IOCAFbits.IOCAF2 = 0;
        /**
         * Do nothing and wait for instructions 
         * over uart, ic2 etc. in main routine.
         */
    } else if (PIR1bits.RCIF == 1) {    // handle over UART received char
        // status flag is cleard by hardware
        
        // read FIFO of the uart module
        while(PIR1bits.RCIF == 1) {
            // RCIF flag is cleared by reading RCREG register.

            if (RCSTAbits.FERR == 1) {  // invalid stop bit received?
                // ignore this character
                RCREGbits.RCREG;
                continue;
            }
            // store all characters to the internal ring buffer
            if (buffer_is_full == 1) {  // full buffer?
                // ignore this character
                RCREGbits.RCREG;
                continue;
            }
            ring_buffer[write_pointer] = (char) RCREGbits.RCREG;
            ROTATE_POINTER(&write_pointer);
            if(write_pointer == read_pointer) {
                // buffer is full
                buffer_is_full = 1;
            }
        }
    } else if (PIR1bits.TMR2IF == 1) {  // discharge completed
        // clear status flag
        PIR1bits.TMR2IF = 0;
        // stop discharging
        STOP_TIMER_DISCHARGING();
        SET_GPIO_RF_DISCHARGE(0);
     
#ifndef ENABLE_TOTT
        // go to sleep again
        ENABLE_INTERRUPT_RF_IR(1);
        ongoing_address_sampling = 0;
#else
        // PIC power off
        RELEASE_RF_IR_HOLD();
#endif
    }
    
    return;
}
