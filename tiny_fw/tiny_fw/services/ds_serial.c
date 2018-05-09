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
	
	
	ConfigureSleep(&Timer0, 9600);
	
	// Start Bit
	SetPinValue(TX_PIN, LOW);
	Sleep(&Timer0);
	
	// Send Data
	for(uint8_t i = 0; i < sizeof(data) * BITS_IN_BYTE; i++){
		SetPinValue(TX_PIN, (data & 0x01));
		data >>= 1;
		Sleep(&Timer0);
	}
	
	// Stop Bit
	SetPinValue(TX_PIN, HIGH);
	Sleep(&Timer0);
}

DS_Error SerialSend(uint8_t * data, size_t len){
	DisableINT0();
	if (uart_state != IDLE){
		return UART_BUSY;
	} else{
		uart_state = TRANSMITTING;
	}

	while (len--){
		SerialSendByte(*data++);
	}
	uart_state = IDLE;
	return SUCCESS;
}


void SerialInit (void) {
	//init_timer(serialTimer, SERIAL_TIMER_LABEL);
	MCUCR |= (1 << ISC01) | (0<<ISC00);
	//set_timer0_waveform(TIMER_CTC_MODE);
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
		Timer0.set_time(9600); // TODO only works if inlined
		uart_state = RECEIVING;
	} else if(uart_state == RECEIVING){
		rx_bits++;
		RX_BUFF =  (RX_BUFF >> 1) | (GetPinValue(RX_PIN) << 7);
		if (rx_bits >= (sizeof(RX_BUFF) * BITS_IN_BYTE)){
			rx_bits = 0;
			uartRXPush(RX_BUFF);
			uart_state = IDLE;
			Timer0.stop();
			Timer0.disable_int();
			ClearINT0();
			EnableINT0();
		}
	}
	
}



void uart_rx_int_handler(void){
	uart_state = RECEIVE_START;
	DisableINT0();          // Disable pin change interrupts
	//ConfigureSleep(19200);
	//set_timer0_waveform(TIMER_CTC_MODE);
	Timer0.set_prescaler(2);
	Timer0.set_time(52);
	Timer0.clear_int();
	Timer0.enable_int();
	Timer0.start();
}


ISR(INT0_vect){
	uart_rx_int_handler();	
}
