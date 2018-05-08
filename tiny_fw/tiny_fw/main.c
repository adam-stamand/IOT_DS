/*
 * tiny_fw.c
 *
 * Created: 4/22/2018 3:06:08 AM
 * Author : adams
 */ 

#define F_CPU 8000000UL //Processor Frequency (default)

#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/delay.h>
#include <avr/interrupt.h>    // Needed to use interrupts
#include <assert.h>

//PIN5 saved for reset
#define LED1_PIN	PINB3
#define RX_PIN		PINB2
#define IR_PIN		PINB4
#define SW_PIN		PINB0
#define TX_PIN		PINB1


#define OUTPUT 1
#define INPUT 0

typedef enum {
	TRANSMITTING,
	RECEIVE,
	IDLE
}UartState;

UartState uart_state = IDLE;

void togglePin(uint8_t pin){
	PINB = 1<<pin;
}
uint8_t readPin(uint8_t pin){
	return ((PINB & (1<<pin)) != 0);
}

void configurePin(uint8_t pin, uint8_t dir){
	DDRB = dir ? (DDRB | 1<<pin) : (DDRB & ~(1<<pin));
}

void setPin(uint8_t pin, uint8_t val){
	PORTB = val ? (PORTB | 1<<pin) : (PORTB & ~(1<<pin));
}

volatile uint8_t num_tx_bits;
volatile uint8_t num_rx_bits;
volatile uint16_t TX_BUFF;
volatile uint16_t RX_BUFF;


uint8_t receiving = 0;
uint8_t starting = 0;
ISR(INT0_vect){

	GIMSK&= ~(1<<INT0);          // Disable pin change interrupts
	TIFR |= 1<<OCF0A;             // Clear output compare flag
	TCCR0A = 2<<WGM00;            // CTC mode
	TCCR0B = 0<<WGM02 | 2<<CS00;  // Set prescaler to /8
	TCNT0 = 0;                    // Count up from 0 // set counter to 0
	OCR0A = 52;                  // Delay (51+1)*8 cycles
	TIMSK |= (1<<OCIE0A);          // Enable COMPA interrupt
	starting = 1;

}


void GoToSleep(void){
	sleep_enable();
	sleep_cpu();
	sleep_disable();
}

typedef enum {
	CONF_104US,
	CONF_1MS,
	CONF_52US,
	CONF_500US
}CycleConfig;

void ConfigureCycleSleep(CycleConfig conf){
	GTCCR = (1 << TSM) | (1 << PSR0);
	TCNT0 = 0;                    // Count up from 0 // set counter to 0
	
	TCCR0A = 2<<WGM00;            // CTC mode
	switch(conf){
		case CONF_104US:
			TCCR0B = 0<<WGM02 | 2<<CS00;  // Set prescaler to /8
			OCR0A = 104;                  // Delay (51+1)*8 cycles
			break;
		case CONF_1MS:
			TCCR0B = 0<<WGM02 | 2<<CS00;  // Set prescaler to /8
			OCR0A = 104;                  // Delay (51+1)*8 cycles
			break;
		
		case CONF_52US:
			TCCR0B = 0<<WGM02 | 2<<CS00;  // Set prescaler to /8
			OCR0A = 52;                  // Delay (51+1)*8 cycles
			break;
		
		case CONF_500US:
			TCCR0B = 0<<WGM02 | 2<<CS00;  // Set prescaler to /8
			OCR0A = 104;                  // Delay (51+1)*8 cycles
			break;
		
		default:
			break;
	}
}


void CycleSleep(void){
	//TCNT0 = 0;                    // Count up from 0 // set counter to 0
	
	TIMSK |= (1<<OCIE0A);          // Enable COMPA interrupt
	GoToSleep();
	//GTCCR = (1 << TSM) | (1 << PSR0);
	TIMSK &= ~(1<<OCIE0A);          // Disable COMPA interrupt
}


void UartSendByte(uint8_t data){
	
	if (receiving){
		return;
	}
	uint16_t uart_packet = 0xFFFF;
	uart_packet = (uart_packet << 9) | (data << 1);
	TX_BUFF = uart_packet;
	num_tx_bits = 10;
	
	ConfigureCycleSleep(CONF_104US);
	GTCCR = (0 << TSM) | (0 << PSR0);
	while(num_tx_bits){
		if (TX_BUFF & 0x01){
			PORTB = PINB | (1<<TX_PIN);
		}else{
			PORTB = PINB & ~(1<<TX_PIN);
		}
		
		TX_BUFF = TX_BUFF  >> 1;
		num_tx_bits--;
		CycleSleep();
		if (receiving){
			return;
		}
	}
}

void UartSend(uint8_t * data, uint8_t len){
	while (len--){
		UartSendByte(*data++);
	}
}

int UartReceiveByte(uint8_t * data){
	num_rx_bits = 8;
	*data = 0;
	ConfigureCycleSleep(CONF_52US);
	TCNT0 = 0;
	
	while(readPin(RX_PIN));
	GTCCR = (0 << TSM) | (0 << PSR0);
	togglePin(TX_PIN);
	CycleSleep();
	togglePin(TX_PIN);
	ConfigureCycleSleep(CONF_104US);	
	GTCCR = (0 << TSM) | (0 << PSR0);
	while (num_rx_bits--){
		
		CycleSleep();
		togglePin(TX_PIN);
		*data = (*data >> 1) | readPin(RX_PIN) << 7;
	}
	CycleSleep();
	if (readPin(RX_PIN) != 1){
		return -1;
	}
	return 0;
}


#define RX_CIRC_MAX 100
uint8_t rx_circ_buff[RX_CIRC_MAX];
uint32_t rx_circ_head = 0;
uint32_t rx_circ_tail = 0;


void UartReceive(uint8_t * data, uint8_t len){
	
	while(len--){
		if (rx_circ_tail == rx_circ_head){
			continue;
		}
		*data++ = rx_circ_buff[rx_circ_tail++];
		rx_circ_tail %= RX_CIRC_MAX;
	}
	
	//int rv = 0;
	//while (len--){
	//	rv = UartReceiveByte(data++);
	//	if (rv!=0){
	//		assert(0);
	//	}
	//}
}
uint8_t rx_bits = 0;
uint8_t temp_data = 0;
ISR (TIMER0_COMPA_vect) {
	if(starting){
		OCR0A = 104;  
		receiving = 1;
		starting = 0;
		RX_BUFF = 0;
		return;
	}
	if(receiving){

		rx_bits++;
		RX_BUFF =  (RX_BUFF >> 1) | (readPin(RX_PIN) << 7);
		if (rx_bits >= 8){
			receiving = 0;
			rx_bits = 0;
			rx_circ_buff[rx_circ_head] = RX_BUFF & 0xFF;
			rx_circ_head = (rx_circ_head + 1) % RX_CIRC_MAX;
			GIFR = 1<<INTF0;                 // Clear pin change interrupt flag.
			GIMSK |= 1<<INT0;               // Enable pin change interrupts

		}
	}
	//RX_BUFF++;
}





int readEEPROM(uint8_t addr, uint8_t * data, uint16_t len){
	
	EECR = (0<<EEPM1)|(0<<EEPM0);
	while (len--){
		while(EECR & (1<<EEPE));
		EEAR = addr;
		EECR |= 1<<EEMPE;
		EECR |= (1<<EERE);
		*data++ = EEDR;
	}
	return 0;
}
int writeEEPROM(uint8_t addr, uint8_t * data, uint16_t len){
	
	EECR = (0<<EEPM1)|(0<<EEPM0);
	while(len--){
		/* Wait for completion of previous write */
		while(EECR & (1<<EEPE));
		/* Set up address and data registers */
		EEAR = addr;
		EEDR = *data++;
		/* Write logical one to EEMPE */
		EECR |= (1<<EEMPE);
		/* Start eeprom write by setting EEPE */
		EECR |= (1<<EEPE);
	}
	return 0;
}

void function(int* x){
	uint8_t dst, src = (*x + 5);
	asm (	"mov %1, %0"
			: "=r" (dst)
			: "r" (src)	);
	*x = dst;
}

void program1(void) __attribute__ ((section (".program1")));

void program1(void){
	uint8_t data_eeprom = 0x01;
	//GIMSK |= 1<<PCIE;
	
	//PCMSK |= 1<<PCINT2;
	writeEEPROM(0x01, &data_eeprom, 1);
	num_tx_bits = 0;

		//TIMSK |= 1<<OCIE0A;           // Enable output compare interrupt
	uint8_t data[] = {0x01,0x02,0xAA,0x55};
	while(1){
		//x = readPin(IR_PIN);
		//if (x){
		//setPin(LED1_PIN, 0);
		
		//}
// 		if (num_tx_bits == 0){
 //			num_tx_bits = 20;
 //			TX_BUFF = 0xFFFCA;
 			//togglePin(LED1_PIN);
 //		}
		UartReceive(data, 1);
		if (data[0] == 0xB0){
			//togglePin(LED1_PIN);
		}
		//data[0] = rx_circ_buff[rx_circ_head-1];
		//UartSend(&temp_data, 1);
		data[1] = 0x0d;

		UartSend(data, 1);
		//UartReceive(data, sizeof(data));
 		_delay_ms(500);
	}
}


void program2(void) __attribute__ ((section (".program2")));

void program2(void){
	uint8_t data = 0x00;
	writeEEPROM(0x01, &data, 1);
	while(1){
		//togglePin(LED2_PIN);
		_delay_ms(500);
	}
}


void UartInit (void) {

	USICR = 0;                      // Disable USI.

	
	//PCMSK = 1<<PCINT0;             // Enable pin change on pin 0
	
	configurePin(TX_PIN, OUTPUT);
	setPin(TX_PIN, 1);
}


int main(void)
{
		cli();

		MCUCR = (1 << ISC01) | (0<<ISC00);
	configurePin(RX_PIN, INPUT);         // Define DI as input
		
//_delay_ms(500);

	GIFR = 1<<INTF0;                 // Clear pin change interrupt flag.
	GIMSK |= 1<<INT0;               // Enable pin change interrupts
	//GIFR = 1<<INTF0;                 // Clear pin change interrupt flag.
	//_delay_ms(500);
	sei();
	UartInit();
	//MCUCR |= (0 <<SM1) | (0 << SM0);
	uart_state = IDLE;


    /* Replace with your application code */
    //volatile uint8_t boot_flag = 0;
	
	configurePin(LED1_PIN, OUTPUT);
	//configurePin(SW_PIN, OUTPUT);
	//configurePin(IR_PIN, INPUT);
	
	receiving = 0;
	
	//readEEPROM(0x01, &boot_flag, 1);

	//if (boot_flag == 0){
		program1();
	//}else{
	//	program2();
	//}
	
}

