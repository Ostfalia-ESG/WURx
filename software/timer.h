#ifndef TIMER_H
#define	TIMER_H

#ifdef	__cplusplus
extern "C" {
#endif

    /**
     * Initilize the timer 1 for the address bit sampling.
     */
    void init_timer_sampling(void);
    
#define ENABLE_TIMER_SAMPLING()   \
        T1CONbits.TMR1ON = 1;           \
    
#define DISABLE_TIMER_SAMPLING()    \
    T1CONbits.TMR1ON = 0;           \
    TMR1Hbits.TMR1H = 0x00;         \
    TMR1Lbits.TMR1L = 0x00;         \

#define TIMER_START_VALUE(X)          \
    TMR1 = (X)


    /**
     * Initilize the timer 2 for discharging
     */
    void init_timer_discharge(void);
    
    /**
     * To start or stop the discharging timer.
     * The target time is set in 'configuration.c'.
     */
#define START_TIMER_DISCHARGING()  \
    T2CONbits.TMR2ON = 1;
    
#define STOP_TIMER_DISCHARGING()    \
    T2CONbits.TMR2ON = 0;          \
    TMR2 = 0;                       
    
    
#ifdef	__cplusplus
}
#endif

#endif	/* TIMER_H */

