/*
 * ds_serial.c
 *
 * Created: 5/8/2018 2:04:50 PM
 *  Author: amand
 */ 

#include "drivers/drivers.h"
#include "services/services.h"
#include "ds_conf.h"

CircularBuffer uartRXBuffer = {.head = 0, .tail = 0, .buffer = {0}};

UartState uart_state = IDLE;



DS_Error uartRXPop(uint8_t* data){
	if (_pop_circ_buffer(&uartRXBuffer, data) != 0){
		return RX_EMPTY;
	}
	return SUCCESS;
}

bool uartRXPush(uint8_t data){
	return _push_circ_buffer(&uartRXBuffer, data);
}

size_t uartRXBytesUsed(void){
	return _get_bytes_used(&uartRXBuffer);
}


void SerialSendByte(uint8_t data){
	
	
	ConfigureSleep(CONF_104US);
	
	// Start Bit
	SetPinValue(TX_PIN, LOW);
	Sleep();
	
	// Send Data
	for(uint8_t i = 0; i < sizeof(data) * BITS_IN_BYTE; i++){
		SetPinValue(TX_PIN, (data & 0x01));
		data >>= 1;
		Sleep();
	}
	
	// Stop Bit
	SetPinValue(TX_PIN, HIGH);
	Sleep();
}

DS_Error SerialSend(uint8_t * data, uint8_t len){
	if (uart_state != IDLE){
		return UART_BUSY;
	}

	while (len--){
		SerialSendByte(*data++);
	}
	return SUCCESS;
}


void SerialInit (void) {
	MCUCR |= (1 << ISC01) | (0<<ISC00);
	SetPinDirection(RX_PIN, INPUT);         // Define DI as input
	SetPinDirection(TX_PIN, OUTPUT);
	SetPinValue(TX_PIN, HIGH);
	uart_state = IDLE;
	ClearINT0();
	EnableINT0();
}





size_t SerialReceive(uint8_t * data, size_t len){
	size_t i = 0;
	for (i = 0; i < len; i++){
		if (uartRXPop(data++) == RX_EMPTY){
			break;
		}
	}
	return i;
}









ISR (TIMER0_COMPA_vect) {
	static uint8_t RX_BUFF = 0;
	static uint8_t rx_bits = 0;
	
	if(uart_state == RECEIVE_START){
		OCR0A = 104;
		uart_state = RECEIVING;
		return;
	}
	
	if(uart_state == RECEIVING){
		rx_bits++;
		RX_BUFF =  (RX_BUFF >> 1) | (GetPinValue(RX_PIN) << 7);
		if (rx_bits >= (sizeof(RX_BUFF) * BITS_IN_BYTE)){
			rx_bits = 0;
			uartRXPush(RX_BUFF);
			uart_state = IDLE;
			ClearINT0();
			EnableINT0();
			stop_timer0();
		}
	}
}



void uart_rx_int_handler(void){
	DisableINT0();          // Disable pin change interrupts
	ConfigureSleep(9600);
	uart_state = RECEIVE_START;
	start_timer0();
}


ISR(INT0_vect){
	uart_rx_int_handler();	
}
