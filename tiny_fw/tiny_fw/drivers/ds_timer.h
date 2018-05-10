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
	TIMER_CTC_MODE = 0x02,
} TimerWaveForm;


typedef struct _Timer{
	void (*const set_waveform)(uint8_t mode);
	void (*const set_prescaler)(uint8_t mode);
	void (*const set_compare)(uint8_t val);
	void (*const set_controlA)(uint8_t mode);
	void (*const set_controlB)(uint8_t mode);
	void (*const set_time)(uint8_t val);
	void (*const start)(void);
	void (*const stop)(void);
	void (*const enable_int)(void);
	void (*const disable_int)(void);
	void (*const clear_int)(void);
	void (*const set_interval)(long unsigned int val, struct _Timer* timer);
} Timer;

extern Timer Timer0;
extern Timer Timer1;






#endif /* DS_TIMER_H_ */