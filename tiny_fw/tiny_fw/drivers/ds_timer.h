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

inline void set_timer0_waveform(uint8_t mode){
	TCCR0A = (TCCR0A & ~DS_WAVEFORM_MASK ) | (mode & DS_WAVEFORM_MASK );           
}

inline void set_timer0_compare(uint8_t val){
	OCR0A = val;
}

inline void set_timer0_control(uint8_t val){
	TCCR0A = val;
}

inline void set_timer0(uint8_t val){
	TCNT0 = val;
}

extern uint16_t prescaler_buf[];

inline void set_timer0_time(long unsigned int val){
	long unsigned int cycles = 0;
	long unsigned int temp = F_CPU / val;
	
	uint8_t cnt = 0;
	
	while ((cycles > 255) && (cnt++ < 5)){ // TODO replace 5
		cycles = (temp >> prescaler_buf[cnt]);
	}
	
	if (cnt >= 5){
		assert(0);
	}
	
	set_timer0_control(cnt+1);
	set_timer0_compare(cycles);
}


inline void start_timer0(void){
	GTCCR = (0 << TSM) | (0 << PSR0);
}

inline void stop_timer0(void){
	GTCCR = (1 << TSM) | (1 << PSR0);
}


inline void enable_timer0_int(void){
	TIMSK |= (1<<OCIE0A);
}

inline void disable_timer0_int(void){
	TIMSK &= ~(1<<OCIE0A);          // Disable COMPA interrupt
}

inline void clear_timer0_int(void){
	TIFR |= 1<<OCF0A;             // Clear output compare flag
}



#endif /* DS_TIMER_H_ */