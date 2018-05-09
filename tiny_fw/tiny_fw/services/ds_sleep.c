/*
 * ds_sleep.c
 *
 * Created: 5/8/2018 1:39:03 PM
 *  Author: amand
 */ 

#include "drivers/drivers.h"
#include "services/ds_sleep.h"





void ConfigureSleep(long unsigned int time_hz){
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


