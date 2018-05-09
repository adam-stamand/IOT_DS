/*
 * tiny_fw.c
 *
 * Created: 4/22/2018 3:06:08 AM
 * Author : adams
 */ 

#include "ds_conf.h"
#include "services/services.h"
#include "MQTTSNPacket.h"
#include <string.h>
#include <assert.h>
#include <stdbool.h>




ImageSelect GetBootImageSelect(void){
	ImageSelect sel;
	ReadEEPROM(BOOT_ADDR, (uint8_t*)&sel, sizeof(ImageSelect));
	return sel;
}

void SetBootImageSelect(ImageSelect sel){
	WriteEEPROM(BOOT_ADDR, (uint8_t*)&sel, sizeof(ImageSelect));
}

void Print(char* str){
	SerialSend((uint8_t*)str, strlen(str));
}

void program1(void) __attribute__ ((section (".program1")));
void program1(void){
	
	//SetBootImageSelect(PROGRAM2);

	uint8_t data[CIRC_MAX_SZ];
	while(1){
		//x = readPin(IR_PIN);
		//if (x){
		//setPin(LED1_PIN, 0);	
		//}
	
		size_t temp = SerialReceive(data, CIRC_MAX_SZ);
		char temp_str[100] = {0};
		itoa(temp, temp_str, 10);
		temp_str[100] = '\0';
		Print(temp_str);
		data[temp] = '\0'; // terminate string
		Print(data);
		TogglePin(LED1_PIN);
		Print("Hello, I am the Door Sensor!\n");
 		_delay_ms(3000);
	}
}


void program2(void) __attribute__ ((section (".program2")));
void program2(void){
	//SetBootImageSelect(PROGRAM1);
	while(1){
		_delay_ms(500);
	}
}



//TODO add watchdog

int main(void)
{

	disableGlobalInt();
	SerialInit();
	enableGlobalInt();
	
	SetPinDirection(LED1_PIN, OUTPUT);
	SetPinDirection(SW_PIN, OUTPUT);
	SetPinDirection(IR_PIN, INPUT);	

    ImageSelect sel = GetBootImageSelect();
	sel = PROGRAM1;

	switch(sel){
		
		case PROGRAM1:
			program1();
			break;
		
		case PROGRAM2:
			program2();
			break;
		
		default:
			assert(0);
		
	}
	
}

