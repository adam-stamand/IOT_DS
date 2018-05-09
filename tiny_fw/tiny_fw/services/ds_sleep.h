/*
 * ds_sleep.h
 *
 * Created: 5/8/2018 2:10:57 PM
 *  Author: amand
 */ 


#ifndef DS_SLEEP_H_
#define DS_SLEEP_H_

#include "drivers/drivers.h"

typedef enum {
	TIMER_CTC_MODE = 0x02,
} TimerWaveForm;



//void ConfigureSleep(long unsigned int time_hz);





inline void ConfigureSleep(long unsigned int time_hz){
	set_timer0(0);                    // Count up from 0 // set counter to 0
	set_timer0_waveform(TIMER_CTC_MODE);
	
	set_timer0_time(time_hz);
	/*
	switch(conf){
		case CONF_104US:
		TCCR0B = 0<<WGM02 | 2<<CS00;  // Set prescaler to /8
		OCR0A = 104;                  // Delay (51+1)*8 cycles
		break;
		case CONF_1MS:
		TCCR0B = 0<<WGM02 | 2<<CS00;  // Set prescaler to /8
		OCR0A = 104;                  // Delay (51+1)*8 cycles
		break;
		
		case CONF_52US:
		TCCR0B = 0<<WGM02 | 2<<CS00;  // Set prescaler to /8
		OCR0A = 52;                  // Delay (51+1)*8 cycles
		break;
		
		case CONF_500US:
		TCCR0B = 0<<WGM02 | 2<<CS00;  // Set prescaler to /8
		OCR0A = 104;                  // Delay (51+1)*8 cycles
		break;
		
		default:
		
		break;
	}
	*/
}

inline void Sleep(void){
	enable_timer0_int();
	start_timer0();
	sleep();
	stop_timer0();
	disable_timer0_int();
}



#endif /* DS_SLEEP_H_ */