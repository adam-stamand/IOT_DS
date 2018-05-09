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



void ConfigureSleep(long unsigned int time_hz);


inline void Sleep(void){
	enable_timer0_int();
	start_timer0();
	sleep();
	stop_timer0();
	disable_timer0_int();
}



#endif /* DS_SLEEP_H_ */