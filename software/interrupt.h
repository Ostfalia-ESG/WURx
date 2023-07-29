#ifndef INTERRUPT_H
#define	INTERRUPT_H

#ifdef	__cplusplus
extern "C" {
#endif

    /**
     * Initilize all interrupts in general (for on-change and peripherials).
     */
    void init_interrupts(void);

    /**
     * To enable or disable the interrupt on positive edge.
     * @param value 1=enabled, 0=disabled
     */
#define ENABLE_INTERRUPT_RF_IR(value)   \
    IOCAPbits.IOCAP5 = value;   // positive edge for pin A5
    
     /**
     * Activates the interrupt for a negative edge on the data pin (sync bit).
     * @param value 1=enabled, 0=disabled
     */
#define ENABLE_INTERRUPT_RF_DATA(value)   \
    IOCANbits.IOCAN4 = value;    // negative edge for pin A4
    
    /**
     * To enable or disable the interrupt on positive edge.
     * @param value 1=enabled, 0=disabled
     */
#define ENABLE_INTERRUPT_RB_IR_CONF_EN(value)   \
    IOCANbits.IOCAN2 = value;   // negative edge for pin A2

    
    
    
#ifdef	__cplusplus
}
#endif

#endif	/* INTERRUPT_H */

