/*
 * ds_timer.c
 *
 * Created: 5/8/2018 1:35:42 PM
 *  Author: amand
 */ 


#include "drivers/ds_timer.h"


	
	
	
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



static void set_timer1_waveform(uint8_t mode){
	TCCR0A = (TCCR0A & ~DS_WAVEFORM_MASK ) | (mode & DS_WAVEFORM_MASK );
}

static void set_timer1_prescaler(uint8_t mode){
	TCCR0B = (TCCR0B & ~DS_PRESCALER_MASK ) | (mode & DS_PRESCALER_MASK);
}

static void set_timer1_compare(uint8_t val){
	OCR0A = val;
}

static void set_timer1_controlA(uint8_t val){
	TCCR0A = val;
}

static void set_timer1_controlB(uint8_t val){
	TCCR0B = val;
}

static void set_timer1(uint8_t val){
	TCNT0 = val;
}

static void start_timer1(void){
	GTCCR = (0 << TSM) | (0 << PSR0);
}

static void stop_timer1(void){
	GTCCR = (1 << TSM) | (1 << PSR0);
}

static void enable_timer1_int(void){
	TIMSK |= (1<<OCIE0A);
}

static void disable_timer1_int(void){
	TIMSK &= ~(1<<OCIE0A);          // Disable COMPA interrupt
}

static void clear_timer1_int(void){
	TIFR |= 1<<OCF0A;             // Clear output compare flag
}


//extern uint16_t prescaler_buf[];
static uint8_t prescaler_buf[] = {0, 3, 6, 8, 10};
void set_timer_interval(long unsigned int val, Timer* timer){

	long unsigned int temp = F_CPU / val;
	long unsigned int cycles = temp;
	uint8_t cnt = 0;
	
	while ((cycles > 255) && (cnt++ < 5)){ // TODO replace 5
		cycles = (temp >> prescaler_buf[cnt]);
	}
	
	if (cnt >= 5){
		//assert(0);
	}
	timer->set_prescaler(cnt+1);
	timer->set_compare(cycles);

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
	.clear_int = clear_timer0_int,
	.set_interval = set_timer_interval
	
};

Timer Timer1 = {
	.set_waveform = set_timer1_waveform,
	.set_prescaler =  set_timer1_prescaler,
	.set_compare = set_timer1_compare,
	.set_controlA =  set_timer1_controlA,
	.set_controlB = set_timer1_controlB,
	.set_time = set_timer1,
	.start = start_timer1,
	.stop = stop_timer1,
	.enable_int = enable_timer1_int,
	.disable_int = disable_timer1_int,
	.clear_int = clear_timer1_int,
	.set_interval = set_timer_interval
};


