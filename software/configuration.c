#include <stdio.h>
#include <stdlib.h>
#include <xc.h>

#include "main.h"
#include "uart.h"
#include "configuration.h"

typedef enum configuration_status_e {COMMAND, ARGUMENTS} configuration_status_et;
volatile configuration_status_et conf_status = COMMAND;

WurConfiguration wur_config;


uint8_t last_command = 0;
uint8_t address_arg_num = 0;

// Max baud rate possible due to the runtime needed by the pic to sample one bit
#define MAX_BAUD_RATE_HARD(freq) (freq / (CRITICAL_PATH_INST * 4.0 * 1.0))
// Max baud rate possible due to the runtime needed by the pic to sample one bit(with 25 % tolerance)
#define MAX_BAUD_RATE_SOFT(freq) (MAX_BAUD_RATE_HARD(freq) / TOLERANCE)
// Max baud rate argument possible for one Fosc frequency
#define MAX_BAUD_RATE_ARG(freq) (uint16_t)((MAX_BAUD_RATE_SOFT(freq) / 100.0) - 1.0)
//#define MAX_BAUD_RATE_ARG(freq) (MAX_BAUD_RATE_ARG_TEST(freq) < 0 ? (int16_t) -1 : (int16_t) MAX_BAUD_RATE_ARG_TEST(freq))

#define FREQ_31_KHZ 31000
#define FREQ_62_5_KHZ 62500
#define FREQ_125_KHZ 125000
#define FREQ_250_KHZ 250000
#define FREQ_500_KHZ 500000
#define FREQ_1_MHZ 1000000
#define FREQ_2_MHZ 2000000
#define FREQ_4_MHZ 4000000
#define FREQ_8_MHZ 8000000
#define FREQ_16_MHZ 16000000
/**
 * Timer target value:
 * value = freq / baud_rate - 1 = freq / (100 * (Arg + 1) * 4) - 1
 * To minimize ressources, the freq is precalculated to freq_value
 * freq_value = freq / 400 
 */
#define FREQ_VALUE(freq) freq / 400

uint16_t config_target_timer_value;
uint16_t config_target_sync_timer_value;
uint8_t config_osccon_bits;

// 60us discharging time (1 / (37us * 100))
#define DISCHARGING_TIME_FREQ 270

void update_time_based_params(uint16_t freq) {
    // timer1 configuration for sampling
    config_target_timer_value = (freq / (wur_config.bit_rate_arg + 1)) - 1;
    config_target_sync_timer_value = (config_target_timer_value >> 1) + SYNC_BIT_TIMER_COMPENSATION;
    
    if (config_target_sync_timer_value >= config_target_timer_value) {
        config_target_sync_timer_value = config_target_timer_value;
    }
    
    // set target compare value (with respect Fosc/4)
    CCPR1Lbits.CCPR1L = (uint8_t) config_target_timer_value;
    CCPR1Hbits.CCPR1H = (uint8_t) (config_target_timer_value >> 8);
    
    // set discharging timer period for timer 2
    PR2bits.PR2 = (uint8_t) (freq / DISCHARGING_TIME_FREQ);
}

void init_wur_configuration(uint8_t update_oscilator_clk) {
    // Set minimal oscilator frequency and params for given baudrate
    if (wur_config.bit_rate_arg <= MAX_BAUD_RATE_ARG(FREQ_31_KHZ)) {            // 31 kHz (LF))
        config_osccon_bits = 0b0000;
        update_time_based_params(FREQ_VALUE(FREQ_31_KHZ));
        // uart configuration
        // 4x multiplier
        TXSTAbits.BRGH = 1;
        BAUDCONbits.BRG16 = 1;
        SPBRGLbits.SPBRGL = (uint8_t) 24;
        SPBRGHbits.SPBRGH = (uint8_t)(24 >> 8);
    } else if (wur_config.bit_rate_arg <= MAX_BAUD_RATE_ARG(FREQ_62_5_KHZ)) {   // 62.5 kHz (MF)
        config_osccon_bits = 0b0100;
        update_time_based_params(FREQ_VALUE(FREQ_62_5_KHZ));
        // uart configuration
        // 4x multiplier
        TXSTAbits.BRGH = 1;
        BAUDCONbits.BRG16 = 1;
        SPBRGLbits.SPBRGL = (uint8_t) 49;
        SPBRGHbits.SPBRGH = (uint8_t)(49 >> 8);
    } else if (wur_config.bit_rate_arg <= MAX_BAUD_RATE_ARG(FREQ_125_KHZ)) {    // 125 kHz (MF)
        config_osccon_bits = 0b0101;
        update_time_based_params(FREQ_VALUE(FREQ_125_KHZ));
        // uart configuration
        // 4x multiplier
        TXSTAbits.BRGH = 1;
        BAUDCONbits.BRG16 = 1;
        SPBRGLbits.SPBRGL = (uint8_t) 99;
        SPBRGHbits.SPBRGH = (uint8_t)(99 >> 8);
    } else if (wur_config.bit_rate_arg <= MAX_BAUD_RATE_ARG(FREQ_250_KHZ)) {    // 250 kHz (MF)
        config_osccon_bits = 0b0110;
        update_time_based_params(FREQ_VALUE(FREQ_250_KHZ));
        // uart configuration
        // 4x multiplier
        TXSTAbits.BRGH = 1;
        BAUDCONbits.BRG16 = 1;
        SPBRGLbits.SPBRGL = (uint8_t) 200;
        SPBRGHbits.SPBRGH = (uint8_t)(200 >> 8);
    }else if (wur_config.bit_rate_arg <= MAX_BAUD_RATE_ARG(FREQ_500_KHZ)) {     // 500 kHz (MF)
        config_osccon_bits = 0b0111;
        update_time_based_params(FREQ_VALUE(FREQ_500_KHZ));
        // uart configuration
        // 4x multiplier
        TXSTAbits.BRGH = 1;
        BAUDCONbits.BRG16 = 1;
        SPBRGLbits.SPBRGL = (uint8_t) 402;
        SPBRGHbits.SPBRGH = (uint8_t)(402 >> 8);
    } else if (wur_config.bit_rate_arg <= MAX_BAUD_RATE_ARG(FREQ_1_MHZ)) {      // 1 MHz (MF)
        config_osccon_bits = 0b1011;
        update_time_based_params(FREQ_VALUE(FREQ_1_MHZ));
        // uart configuration
        // 4x multiplier
        TXSTAbits.BRGH = 1;
        BAUDCONbits.BRG16 = 1;
        SPBRGLbits.SPBRGL = (uint8_t) 805;
        SPBRGHbits.SPBRGH = (uint8_t)(805 >> 8);
    } else if (wur_config.bit_rate_arg <= MAX_BAUD_RATE_ARG(FREQ_2_MHZ)) {      // 2 MHz (MF)
        config_osccon_bits = 0b1100;
        update_time_based_params(FREQ_VALUE(FREQ_2_MHZ));
        // uart configuration
        // 4x multiplier
        TXSTAbits.BRGH = 1;
        BAUDCONbits.BRG16 = 1;
        SPBRGLbits.SPBRGL = (uint8_t) 1611;
        SPBRGHbits.SPBRGH = (uint8_t)(1611 >> 8);
    } else if(wur_config.bit_rate_arg <= MAX_BAUD_RATE_ARG(FREQ_4_MHZ)) {      // 4 MHz (MF)
        config_osccon_bits = 0b1101;
        update_time_based_params(FREQ_VALUE(FREQ_4_MHZ));
        // uart configuration
        // 16x multiplier
        TXSTAbits.BRGH = 0;
        BAUDCONbits.BRG16 = 1;
        SPBRGLbits.SPBRGL = (uint8_t) 805;
        SPBRGHbits.SPBRGH = (uint8_t)(805 >> 8);
    } else {      // 8 MHz (MF)
        config_osccon_bits = 0b1110;
        update_time_based_params(FREQ_VALUE(FREQ_8_MHZ));
        // uart configuration
        // 16x multiplier
        TXSTAbits.BRGH = 0;
        BAUDCONbits.BRG16 = 1;
        SPBRGLbits.SPBRGL = (uint8_t) 1611;
        SPBRGHbits.SPBRGH = (uint8_t)(1611 >> 8);
    }
//    } else {                                                                    // 16 MHz (MF)
//        OSCCONbits.IRCF = 0b1111;
//        update_time_based_params(FREQ_VALUE(FREQ_16_MHZ));
//        // uart configuration
//        // 16x multiplier
//        TXSTAbits.BRGH = 0;
//        BAUDCONbits.BRG16 = 1;
//        SPBRGLbits.SPBRGL = (uint8_t) 3224;
//        SPBRGHbits.SPBRGH = (uint8_t)(3224 >> 8);
//    }
     
    if (update_oscilator_clk) {
        OSCCONbits.IRCF = config_osccon_bits;
    }
}

void confirm_configuration(void) {
    // save changes
    write_config_to_eeprom();
    // confirm configuration
    uart_send_char(0x01);
    // wailt until char is transmitted (before going deep sleep)
    WAIT_UNTIL_TX_COMPLETE();
    conf_status = COMMAND;
}

void invalid_argument(void) {
     uart_send_char(0x11);
     conf_status = COMMAND;
     // wailt until char is transmitted (before going deep sleep)
     WAIT_UNTIL_TX_COMPLETE();
}

void conf_loop(void) {
    if (uart_new_character_available()) {
        char c = uart_get_new_character();
        
        if (conf_status == COMMAND) {  // current symbol is a command
            if (c > 0x02) {
                // invalid command
                uart_send_char(0x10);
                // wailt until char is transmitted (before going deep sleep)
                WAIT_UNTIL_TX_COMPLETE();
                return;
            }
            last_command = (uint8_t) c;
            conf_status = ARGUMENTS;
            
        } else if (conf_status == ARGUMENTS) { // current symbol is an argument
            if (last_command == 0x00) { // arguments for address length
                if (c > MAX_ADDRESS_LENGTH_BITS) {
                    invalid_argument();
                    return;
                }
                // set new address length
                wur_config.address_length = (uint8_t) c;
                confirm_configuration();
                return;
                
            } else if (last_command == 0x01) {  // arguments for the new address
                if (address_arg_num == 0) { // is first adress byte?
                    // remove old address
                    wur_config.address = 0;
                    if (wur_config.address_length == 0) {
                        invalid_argument();
                        return;
                    }
                } else {
                    wur_config.address <<= 8;
                }
                
                wur_config.address |= ((uint64_t) c);
                address_arg_num++;
                
                if (address_arg_num == ((wur_config.address_length - 1) >> 3) + 1) { // ceil(wur_address_length /8)
                    address_arg_num = 0;
                    confirm_configuration();
                    return;
                }
            } else if (last_command == 0x02) {
                // set new bit rate
                wur_config.bit_rate_arg = (uint8_t) c;
                confirm_configuration();
                // reinit required to achieve different bit rates
                init_wur_configuration(1);
                return;
            } // invalid commands are catched earlier
        } 
   }
}

/**
 * 256 Bytes EEPROm von 00h bis ffh Adressierbar
 */
void write_config_to_eeprom(void) {   
    // select data eeprom
    EECON1bits.EEPGD = 0;
    // select data eeprom
    EECON1bits.CFGS = 0;
    // enable writes
    EECON1bits.WREN = 1;
    // disable interrupts
    if (INTCONbits.GIE) {
        STATUSbits.CARRY = 1;
    }
    
    uint8_t* config_p = (uint8_t*) &wur_config;
    for (uint8_t i = 0; i < sizeof(WurConfiguration); i++) {
        while (EECON1bits.WR) { 				
            continue; 							
        } 
        // address to write
        EEADRL = i;
        // data to write
        EEDATL = config_p[i];
        
        // specifiy byte sequence to write to EEPROM
        EECON2bits.EECON2 = 0x55;
        EECON2bits.EECON2 = 0xAA;
        EECON1bits.WR = 1;  // set write bit to begin write
    }
    
    // enable interrupts
    if (STATUSbits.CARRY) {
        INTCONbits.GIE = 1;
    }
    // disable writes
    EECON1bits.WREN = 0;
}




void read_config_from_eeprom(void) {
    // select data eeprom
    EECON1bits.EEPGD = 0;
    // select data eeprom
    EECON1bits.CFGS = 0;
    
    uint8_t* config_p = (uint8_t*) &wur_config;
    for (uint8_t i = 0; i < sizeof(WurConfiguration); i++) {
        // address to read
        EEADRL = i;
        // start reading (cleared by hardware)
        EECON1bits.RD = 1;
        config_p[i] = EEDATL;
    }
}

