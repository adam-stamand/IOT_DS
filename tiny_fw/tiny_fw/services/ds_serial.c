/*
 * ds_serial.c
 *
 * Created: 5/8/2018 2:04:50 PM
 *  Author: amand
 */ 

#include "drivers/drivers.h"
#include "services/services.h"
#include "ds_conf.h"

static uint8_t temp_buffer[SERIAL_BUFF_SZ] = {0};
CircularBuffer rxBuffer = {.head = 0, .tail = 0, .buffer = temp_buffer, .buffer_sz = SERIAL_BUFF_SZ};
UartState uart_state = IDLE;
static Timer* serialTimer = &Timer0;

static DS_Error PopRXData(uint8_t* data);
static bool PushRXData(uint8_t data);
static size_t GetRXUsedBytes(void);


inline void DisableRX(void){
	disable_int0();
}

inline void EnableRX(void){
	enable_int0();
}

static void serial_send_byte(uint8_t data){
	ConfigureSleep(serialTimer, 9600);
	
	// Start Bit
	SetPinValue(TX_PIN, LOW);
	Sleep(serialTimer);
	
	// Send Data
	for(size_t i = 0; i < sizeof(data) * BITS_IN_BYTE; i++){
		SetPinValue(TX_PIN, (data & 0x01));
		data >>= 1;
		Sleep(serialTimer);
	}
	
	// Stop Bit
	SetPinValue(TX_PIN, HIGH);
	Sleep(serialTimer);
}

DS_Error SerialSend(uint8_t * data, size_t len){
	DisableRX();
	if (uart_state != IDLE){
		EnableRX();
		return UART_BUSY;
	} 
	uart_state = TRANSMITTING;
	EnableRX();

	while (len--){
		serial_send_byte(*data++);
	}
	uart_state = IDLE;
	return SUCCESS;
}

size_t SerialReceive(uint8_t * data, size_t len){
	size_t i = 0;
	for (i = 0; i < len; i++){
		if (PopRXData(data++) == RX_EMPTY){
			break;
		}
	}
	return i;
}


void SerialInit (void) {
	MCUCR |= (1 << ISC01) | (0<<ISC00);
	serialTimer->set_waveform(TIMER_CTC_MODE);
	SetPinDirection(RX_PIN, INPUT);         // Define DI as input
	SetPinDirection(TX_PIN, OUTPUT);
	SetPinValue(TX_PIN, HIGH);
	uart_state = IDLE;
	ClearINT0();
	EnableRX();
}






ISR (TIMER0_COMPA_vect) {
	static uint8_t RX_BUFF = 0;
	static uint8_t rx_bits = 0;
	
	if(uart_state == RECEIVE_START){
		serialTimer->set_interval(9600, serialTimer);
		uart_state = RECEIVING;
	} else if(uart_state == RECEIVING){
		rx_bits++;
		RX_BUFF =  (RX_BUFF >> 1) | (GetPinValue(RX_PIN) << 7);
		if (rx_bits >= (sizeof(RX_BUFF) * BITS_IN_BYTE)){
			rx_bits = 0;
			PushRXData(RX_BUFF);
			uart_state = IDLE;
			serialTimer->stop();
			serialTimer->disable_int();
			ClearINT0();
			EnableRX();
		}
	}
	
}


static void uart_rx_int_handler(void){
	uart_state = RECEIVE_START;
	DisableRX();          // Disable pin change interrupts
	serialTimer->set_interval(19200, serialTimer);
	serialTimer->clear_int();
	serialTimer->enable_int();
	serialTimer->start();
}

ISR(INT0_vect){
	if (uart_state == IDLE){
		uart_rx_int_handler();		
	}
}



static DS_Error PopRXData(uint8_t* data){
	if (PopCircBuffer(&rxBuffer, data) != 0){
		return RX_EMPTY;
	}
	return SUCCESS;
}

static bool PushRXData(uint8_t data){
	return PushCircBuffer(&rxBuffer, data);
}

static size_t GetRXUsedBytes(void){
	return GetUsedBytes(&rxBuffer);
}
