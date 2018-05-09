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
	stop_timer0();
	set_timer0(0);                    // Count up from 0 // set counter to 0
	set_timer0_waveform(TIMER_CTC_MODE);
	set_timer0_time(time_hz);
	//set_timer0_prescaler(2);
	//set_timer0_compare(104);
}

inline void Sleep(void){
	enable_timer0_int();
	start_timer0();
	sleep();
	disable_timer0_int();
}



#endif /* DS_SLEEP_H_ */