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





inline void ConfigureSleep(Timer* timer, long unsigned int time_hz){
	timer->stop();
	timer->set_time(0);                    // Count up from 0 // set counter to 0
	//set_timer0_waveform(TIMER_CTC_MODE);
	//set_timer0_time(time_hz);
	timer->set_prescaler(2);
	timer->set_compare(104);
}

inline void Sleep(Timer* timer){
	timer->enable_int();
	timer->start();
	sleep();
	timer->disable_int();
}



#endif /* DS_SLEEP_H_ */