/*
 * ds_serial.h
 *
 * Created: 5/8/2018 2:10:26 PM
 *  Author: amand
 */ 


#ifndef DS_SERIAL_H_
#define DS_SERIAL_H_

#include "utils/ds_circular_buffer.h"
#include <avr/interrupt.h>    // Needed to use interrupts
#include <stddef.h>
#include <stdbool.h>

#define BOOT_ADDR 0x30

typedef enum {
	TRANSMITTING,
	RECEIVING,
	RECEIVE_START,
	IDLE
}UartState;

void SerialInit (void);
size_t SerialReceive(uint8_t * data, size_t len);
DS_Error SerialSend(uint8_t * data, size_t len);
void SerialSendByte(uint8_t data);



#endif /* DS_SERIAL_H_ */