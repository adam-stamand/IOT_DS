/*
 * tiny_fw.c
 *
 * Created: 4/22/2018 3:06:08 AM
 * Author : adams
 */ 

#include "ds_conf.h"
#include "services/services.h"
#include "MQTTSNPacket.h"
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






void program1(void) __attribute__ ((section (".program1")));
void program1(void){
	
	//SetBootImageSelect(PROGRAM2);

	uint8_t data[20];
	while(1){
		//x = readPin(IR_PIN);
		//if (x){
		//setPin(LED1_PIN, 0);	
		//}
	
		//GetRXPacket(data);
		
		data[1] = 0x0d;
		SerialSend(data, 1);
		//UartReceive(data, sizeof(data));
 		_delay_ms(500);
	}
}


void program2(void) __attribute__ ((section (".program2")));
void program2(void){
	//SetBootImageSelect(PROGRAM1);
	while(1){
		_delay_ms(500);
	}
}





int main(void)
{

	disableGlobalInt();
	SerialInit();
	enableGlobalInt();
	
	SetPinDirection(LED1_PIN, OUTPUT);
	SetPinDirection(SW_PIN, OUTPUT);
	SetPinDirection(IR_PIN, INPUT);	
	
    ImageSelect sel = GetBootImageSelect();
	
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

