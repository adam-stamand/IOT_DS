/*
 * ds_timer.h
 *
 * Created: 5/8/2018 2:07:29 PM
 *  Author: amand
 */ 


#ifndef DS_TIMER_H_
#define DS_TIMER_H_

#include "ds_conf.h"
#include <assert.h>
#include <avr/io.h>


#define DS_WAVEFORM_MASK ((1 << WGM01) | (1 <<WGM00))
#define DS_PRESCALER_MASK ((1 <<CS02) | (1 <<CS01) | (1 <<CS00))


typedef enum {
	TIMER0,
	TIMER1,
} TimerLabel;

typedef struct _Timer{
	void (*set_waveform)(uint8_t mode);
	void (*set_prescaler)(uint8_t mode);
	void (*set_compare)(uint8_t val);
	void (*set_controlA)(uint8_t mode);
	void (*set_controlB)(uint8_t mode);
	void (*set_time)(uint8_t val);
	void (*start)(void);
	void (*stop)(void);
	void (*enable_int)(void);
	void (*disable_int)(void);
	void (*clear_int)(void);
} Timer;

int init_timer(Timer timer, TimerLabel label);
extern Timer Timer0;

/*
extern uint16_t prescaler_buf[];
inline void set_timer0_time(long unsigned int val){

	long unsigned int temp = F_CPU / val;
	long unsigned int cycles = temp;
	uint8_t cnt = 0;
	
	while ((cycles > 255) && (cnt++ < 5)){ // TODO replace 5
		cycles = (temp >> prescaler_buf[cnt]);
	}
	
	if (cnt >= 5){
		assert(0);
	}
	
	set_timer0_prescaler(cnt+1);
	set_timer0_compare(cycles);
}
*/



#endif /* DS_TIMER_H_ */