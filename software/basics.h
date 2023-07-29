#ifndef BASICS_H
#define	BASICS_H

#ifdef	__cplusplus
extern "C" {
#endif
    /**
     * Put the MCU to deep sleep.
     * The PIC12LF1840 has only one low-power sleep mode.
     */
    inline void do_deep_sleep(void);
    
    /**
     * Initialize the RF_IR pin for detecting the start of a wake-up call.
     */
    void init_gpio_rf_ir(void);
       
    /**
     * Initialize the pin for the combination of the outgoing interrupt 
     * and the ingoing configuration interrupt.
     */
    void init_gpio_rb_ir_conf_en(void);

#define SET_GPIO_RB_IR_CONF_EN()              \
        ENABLE_INTERRUPT_RB_IR_CONF_EN(0);  \
        TRISAbits.TRISA2 = 0; /** set pin A2 as output **/     \
        LATAbits.LATA2 = 0;  // set pin A2 LOW

#define RESET_GPIO_RB_IR_CONF_EN()              \
        TRISAbits.TRISA2 = 1; /** set pin A2 as input **/ \
        ENABLE_INTERRUPT_RB_IR_CONF_EN(1);
    
    /**
     * Initialize the pin for detecting the incomming data.
     */
    void init_gpio_rf_data(void);
    #define READ_RF_DATA() \
        PORTAbits.RA4
   
    /**
     * Initialize the pin for the discharging the rb_ir pin.
     */
    void init_gpio_rf_discharge(void);
    /**
     * To start or stop discharging the frontend on the ib_ir pin.
     */
    #define SET_GPIO_RF_DISCHARGE(value)   \
        LATCbits.LATC1 = value;

    /**
     * Initialize pin for self holding through transistor.
     */
    void init_gpio_rf_ir_hold(void);
    /**
     * To power down the PIC.
     */
    #define RELEASE_RF_IR_HOLD()   \
        LATCbits.LATC0 = 0;
    
    /**
     * Initialize pin for debugging functions.
     */
    void init_gpio_debug_sampling(void);
    #define SET_GPIO_DEBUG_SAMPLING(value)   \
        LATCbits.LATC2 = value;
    
#ifdef	__cplusplus
}
#endif

#endif	/* BASICS_H */

