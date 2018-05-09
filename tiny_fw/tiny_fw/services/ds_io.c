/*
 * ds_io.c
 *
 * Created: 5/8/2018 2:05:21 PM
 *  Author: amand
 */ 


#include "drivers/drivers.h"
#include "services/ds_io.h"

PinValue GetPinValue(IO_Pin pin){
	return (PinValue)read_pin((uint8_t)pin);
}
