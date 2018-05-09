/*
 * ds_io.h
 *
 * Created: 5/8/2018 2:09:45 PM
 *  Author: amand
 */ 


#ifndef DS_IO_H_
#define DS_IO_H_

#include "drivers/drivers.h"



typedef enum{
	LOW = 0,
	HIGH = 1,	
} PinValue;

typedef enum {
	OUTPUT = 1,
	INPUT = 0,
} Pin_Dir;

typedef enum{
	DS_PINB0 = PINB0,
	DS_PINB1 = PINB1,
	DS_PINB2 = PINB2,
	DS_PINB3 = PINB3,
	DS_PINB4 = PINB4,
	DS_PINB5 = PINB5	
} IO_Pin;

typedef enum{
	INT0_LOW_LEVEL = 0x00,
	INT0_ANY_CHANGE = 0x01,
	INT0_FALLING_EDGE = 0x02,
	INT0_RISING_EDGE = 0x03,
} INT0_Config;


inline void SetPinDirection(IO_Pin pin, Pin_Dir dir){
	set_dir_pin((uint8_t)pin, (uint8_t)dir);
}

inline void SetPinValue(IO_Pin pin, PinValue val){
	set_val_pin((uint8_t)pin, (uint8_t)val);
}

inline void TogglePin(IO_Pin pin){
	toggle_pin((uint8_t)pin);
}

PinValue GetPinValue(IO_Pin pin);

inline void EnableINT0(void){
	enable_int0();
}

inline void DisableINT0(void){
	disable_int0();
}

inline void ClearINT0(void){
	clear_int0();
}

inline void ConfigureINT0(INT0_Config cnf){
	config_int0((uint8_t)cnf);
}



#endif /* DS_IO_H_ */