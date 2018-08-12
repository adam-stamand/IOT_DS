/*
 * ds_gpio.h
 *
 * Created: 5/8/2018 2:06:57 PM
 *  Author: amand
 */ 


#ifndef DS_GPIO_H_
#define DS_GPIO_H_
#include <avr/io.h>


inline void toggle_pin(uint8_t pin){
	PINB = 1<<pin;
}

inline uint8_t read_pin(uint8_t pin){
	return ((PINB & (1<<pin)) >> pin);
}

inline void set_dir_pin(uint8_t pin, uint8_t dir){
	DDRB = dir ? (DDRB | 1<<pin) : (DDRB & ~(1<<pin));
}

inline void set_val_pin(uint8_t pin, uint8_t val){
	PORTB = val ? (PORTB | 1<<pin) : (PORTB & ~(1<<pin));
}

inline void clear_int0(void){
	GIFR = 1<<INTF0;                 // Clear pin change interrupt flag
}

inline void enable_int0(void){
	GIMSK |= 1<<INT0;               // Enable pin change interrupt
}

inline void disable_int0(void){
	GIMSK &= ~(1<<INT0);
}

inline void config_int0(uint8_t cnf){
	uint8_t mask = ((1<<ISC01) | (1<<ISC00));
	MCUCR = (MCUCR & ~mask) | (cnf & mask);
}

inline void clear_pcint(void){
	GIFR = 1<<PCIF;                 // Clear pin change interrupt flag
}

inline void enable_pcint(void){
	GIMSK |= 1<<PCIE;               // Enable pin change interrupt
}

inline void disable_pcint(void){
	GIMSK &= ~(1<<PCIE);
}

inline void enable_pcint_mask(uint8_t pin){
	PCMSK |= 1<<pin;               // Enable pin change interrupt
}

inline void disable_pcint_mask(uint8_t pin){
	PCMSK &= ~(1<<pin);               // Enable pin change interrupt
}

#endif /* DS_GPIO_H_ */