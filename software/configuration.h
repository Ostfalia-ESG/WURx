#ifndef CONFIGURATION_H
#define	CONFIGURATION_H

#ifdef	__cplusplus
extern "C" {
#endif
   
#define MAX_ADDRESS_LENGTH_BITS 32
    typedef struct {
        /**
         * Represent the length of the address (between 0 and 64 bit) 
         */
        uint8_t address_length;
        /**
         * Represent the current address.
         */
        uint32_t address;
        /**
         * Represent the current bit rate.
         * 100 bps * (bit_rate_arg + 1)
         */
        uint8_t bit_rate_arg;
    } WurConfiguration;
    extern WurConfiguration wur_config;
    
//#define WUR_BIT_RATE_bps (100 * (wur_config.bit_rate_arg + 1))
    
    extern uint16_t config_target_timer_value;
    extern uint16_t config_target_sync_timer_value;
    extern uint8_t config_osccon_bits;
    
    /**
     * Should be called in the main() loop, for handling the commands send over uart.
     */
    void conf_loop(void);
    
    /**
     * Saves the current config in @wur_config to the eeprom memory.
     */
    void write_config_to_eeprom(void);
    
    /**
     * Reads the current config from the eeprom memory.
     */
    void read_config_from_eeprom(void);
    
    /**
     * Initilize the configurations like adress length, bit_rate etc. (depends on @wur_config)
     * If update_oscilator_clk is zero, the oszillator clk will not be changed.
     */
    void init_wur_configuration(uint8_t update_oscilator_clk);
    
#ifdef	__cplusplus
}
#endif

#endif	/* CONFIGURATION_H */

