/*
 * ds_conf.h
 *
 * Created: 5/8/2018 4:09:00 PM
 *  Author: amand
 */ 


#ifndef DS_CONF_H_
#define DS_CONF_H_


#define F_CPU 8000000UL //Processor Frequency (default)
#define TESTER 0x400


//PIN5 saved for reset
#define LED1_PIN	PINB3
#define RX_PIN		PINB2
#define IR_PIN		PINB4
#define SW_PIN		PINB0
#define TX_PIN		PINB1


#define BITS_IN_BYTE 8





typedef enum {
	CONF_104US,
	CONF_1MS,
	CONF_52US,
	CONF_500US
}CycleConfig;

typedef enum {
	RX_EMPTY,
	UART_BUSY,
	SUCCESS,
}DS_Error;

typedef enum{
	PROGRAM1,
	PROGRAM2,
} ImageSelect;


#endif /* DS_CONF_H_ */