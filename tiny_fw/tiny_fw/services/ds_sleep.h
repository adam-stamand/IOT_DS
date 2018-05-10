/*
 * ds_sleep.h
 *
 * Created: 5/8/2018 2:10:57 PM
 *  Author: amand
 */ 


#ifndef DS_SLEEP_H_
#define DS_SLEEP_H_

#include "drivers/drivers.h"

/*  ConfigureSleep configure the passed in timer to sleep for 1/time_hz seconds
 *  
 *  NOTE: Configuration takes about 3-5us to complete
 */
inline void ConfigureSleep(Timer* timer, long unsigned int time_hz){
	timer->stop();
	timer->set_time(0);                    // Count up from 0 // set counter to 0
	timer->set_interval(time_hz, timer);
}



inline void Sleep(Timer* timer){
	timer->enable_int();
	timer->start();
	sleep();
	timer->disable_int();
}



#endif /* DS_SLEEP_H_ */