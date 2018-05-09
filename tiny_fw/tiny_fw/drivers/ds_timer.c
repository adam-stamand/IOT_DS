/*
 * ds_timer.c
 *
 * Created: 5/8/2018 1:35:42 PM
 *  Author: amand
 */ 


#include "drivers/ds_timer.h"

uint16_t prescaler_buf[] = {1, 3, 6, 8, 10};
	
	
	
static void set_timer0_waveform(uint8_t mode){
	TCCR0A = (TCCR0A & ~DS_WAVEFORM_MASK ) | (mode & DS_WAVEFORM_MASK );
}

static void set_timer0_prescaler(uint8_t mode){
	TCCR0B = (TCCR0B & ~DS_PRESCALER_MASK ) | (mode & DS_PRESCALER_MASK);
}

static void set_timer0_compare(uint8_t val){
	OCR0A = val;
}

static void set_timer0_controlA(uint8_t val){
	TCCR0A = val;
}

static void set_timer0_controlB(uint8_t val){
	TCCR0B = val;
}

static void set_timer0(uint8_t val){
	TCNT0 = val;
}

static void start_timer0(void){
	GTCCR = (0 << TSM) | (0 << PSR0);
}

static void stop_timer0(void){
	GTCCR = (1 << TSM) | (1 << PSR0);
}


static void enable_timer0_int(void){
	TIMSK |= (1<<OCIE0A);
}

static void disable_timer0_int(void){
	TIMSK &= ~(1<<OCIE0A);          // Disable COMPA interrupt
}

static void clear_timer0_int(void){
	TIFR |= 1<<OCF0A;             // Clear output compare flag
}

Timer Timer0 = {			
	.set_waveform = set_timer0_waveform,
	.set_prescaler =  set_timer0_prescaler,
	.set_compare = set_timer0_compare,
	.set_controlA =  set_timer0_controlA,
	.set_controlB = set_timer0_controlB,
	.set_time = set_timer0,
	.start = start_timer0,
	.stop = stop_timer0,
	.enable_int = enable_timer0_int,
	.disable_int = disable_timer0_int,
	.clear_int = clear_timer0_int
};
/*
int init_timer(Timer timer, TimerLabel label){
	switch(label){
		case TIMER0:
			timer.set_waveform = set_timer0_waveform;
			timer.set_prescaler =  set_timer0_prescaler;
			timer.set_compare = set_timer0_compare;
			timer.set_controlA =  set_timer0_controlA;
			timer.set_controlB = set_timer0_controlB;
			timer.set_time = set_timer0;
			timer.start = start_timer0;
			timer.stop = stop_timer0;
			timer.enable_int = enable_timer0_int;
			timer.disable_int = disable_timer0_int;
			timer.clear_int = clear_timer0_int;
			break;
		
		case TIMER1:
			break;
			
		default:
			return -1;
			break;
	}
	
	return 0;
}

*/
