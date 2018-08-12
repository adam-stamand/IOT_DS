/*
 * ds_serial.c
 *
 * Created: 5/8/2018 2:04:50 PM
 *  Author: amand
 */ 

#include "drivers/drivers.h"
#include "services/services.h"
#include "ds_conf.h"


static uint8_t circ_buffer[SERIAL_BUFF_SZ + 1] = {0};
CircularBuffer rxBuffer = {.head = 0, .tail = 0, .buffer = circ_buffer, .buffer_sz = SERIAL_BUFF_SZ + 1}; // +1 to allow room for head and tail to not overlap
volatile UartState uart_state = IDLE;
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
	while(uart_state != IDLE) _delay_ms(100);
	if (uart_state != IDLE){
		EnableRX();
		return UART_BUSY;
	} 
	uart_state = TRANSMITTING;
	volatile uint16_t temp = len;
 	while (temp--){
 		serial_send_byte(*data++);
		//_delay_ms(5);
 	}
 	uart_state = IDLE;
	EnableRX();
	return SUCCESS;
	
}

size_t SerialReceive(uint8_t * data, size_t len){
	size_t i = 0;
	volatile size_t temp = GetRXUsedBytes(); 
	if (temp != len){
		return 0;
	}
	for (i = 0; i < len; i++){
		if (PopRXData(data++) == RX_EMPTY){
			break;
		}
	}
	FlushBuffer(&rxBuffer);
	return i;
}


void SerialInit (void) {
	// MCU Control Register - Falling edge of INT0 causes interrupt
	MCUCR |= (1 << ISC01) | (0<<ISC00);
	// Set timer mode to Clear Timer on Compare
	serialTimer->set_waveform(TIMER_CTC_MODE);
	// Set RX/TX GPIO directions
	SetPinDirection(RX_PIN, INPUT);      
	SetPinDirection(TX_PIN, OUTPUT);
	// Set TX Pin idle high
	SetPinValue(TX_PIN, HIGH);
	// Set UART state
	uart_state = IDLE;
	// Clear any interrupts on INT0
	ClearINT0();
	// Enable the RX
	EnableRX();
	// ??
	for (int i = 0; i < SERIAL_BUFF_SZ; i++){
		rxBuffer.buffer[i] = i;
	}
	
}






ISR (TIMER0_COMPA_vect) {
	static uint8_t RX_BUFF = 0;
	static uint8_t rx_bits = 0;
	
	if(uart_state == RECEIVE_START){
		serialTimer->set_compare(103);
		uart_state = RECEIVING;
		//TogglePin(TX_PIN);

	} else if(uart_state == RECEIVING){
		//TogglePin(TX_PIN);

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
	//ConfigureSleep(serialTimer, 19200);
	//serialTimer->stop();
	//serialTimer->set_interval(19200, serialTimer);
	OCR0A = 52;
	TCCR0B = 2;
	TCNT0 = 0;
	TIMSK |= (1<<OCIE0A);
	GTCCR = (0 << TSM) | (0 << PSR0);
	//serialTimer->set_prescaler(2);
	//serialTimer->set_compare(52);
	//serialTimer->set_time(0);
// 	serialTimer->clear_int();
// 	serialTimer->enable_int();
// 	serialTimer->start();
}

ISR(INT0_vect){
		disable_global_int();
	if (uart_state == IDLE){
		uart_rx_int_handler();		
	}
	enable_global_int();
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
