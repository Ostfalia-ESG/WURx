#ifndef MAIN_H
#define	MAIN_H


/**
 * Usage-specific settings:
 */
#define DEBUG_MSG_ENABLED 1 // Sends debug messages over uart (1=on, 0=off)

#define CRITICAL_PATH_INST 52.0 // number of instructions for the critical path (needed for minimal Fosc)
#define TOLERANCE 1.1
/**
 * Due to varying code sizes, the sampling must be calibrated.
 */
#define SYNC_BIT_INSTRUCTIONS 25
#define SYNC_BIT_TIMER_COMPENSATION SYNC_BIT_INSTRUCTIONS * 1
//#define SYNC_BIT_COMPENSATION_US (1000000 * SYNC_BIT_INSTRUCTIONS * 4 * 2) / _XTAL_FREQ

/**
 * To enable or disble totally-off-trough-transistor (TOTT) mode (later named STFO).
 */
//#define ENABLE_TOTT
#define SHOW_SAMPLING_POINTS


#ifdef	__cplusplus
extern "C" {
#endif
    extern volatile __bit ongoing_address_sampling;
   

#ifdef	__cplusplus
}
#endif

#endif	/* MAIN_H */

