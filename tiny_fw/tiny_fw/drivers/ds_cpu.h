/*
 * ds_cpu.h
 *
 * Created: 5/8/2018 3:58:19 PM
 *  Author: amand
 */ 


#ifndef DS_CPU_H_
#define DS_CPU_H_

#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>    // Needed to use interrupts

inline void enable_global_int(void){
	sei();
}

inline void disable_global_int(void){
	cli();
}

inline void sleep(void){
	sleep_enable();
	sleep_cpu();
	sleep_disable();
}


#endif /* DS_CPU_H_ */