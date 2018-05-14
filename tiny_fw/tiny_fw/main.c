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

typedef enum{
	DOOR_OPEN,
	DOOR_CLOSED
	
} DoorState;

typedef enum{
	OCCUPIED,
	UNOCCUPIED		
	
} OccupationState;


ISR(BADISR_vect){
	while(1){
		TogglePin(LED1_PIN);
		_delay_ms(100);
	}
}


void SerialTest(void){
	uint8_t data2[] = {0xAA, 0x55, 0xAA, 0x55};
	SerialSend(data2, 4);
}

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

DoorState door_state;
OccupationState occupied_state;

void ExecuteStateChange(){
	switch(door_state){
		case DOOR_CLOSED:
			// check IR sensor
			if (occupied_state == UNOCCUPIED){
				// send unoccupied command to gateway
			} else if (occupied_state == OCCUPIED){
				// do nothing
			} else{
				assert(0);
			}
			break;
		
		case DOOR_OPEN:
			if (occupied_state == UNOCCUPIED){
				// send occupied command to gateway
			} else if (occupied_state == OCCUPIED){
				// do nothing
			} else{
				assert(0);
			}
			break;
	}
}

bool CheckChange(bool input){
	static bool temp = false;
	if (temp != input){
		temp = input;
		return true;
	}else{
		return false;
	}
}

#define START_BYTE 0xAA
#define DS_PACKET_SZ 20
uint8_t data[DS_PACKET_SZ] = {0};

#define START_SEQ_SZ 4
#define START_SEQ_OFFSET 0

#define PAYLOAD_LEN_SZ 1
#define PAYLOAD_LEN_OFFSET (START_SEQ_SZ + START_SEQ_OFFSET)

#define PAYLOAD_TYPE_SZ 1
#define PAYLOAD_TYPE_OFFSET (PAYLOAD_LEN_OFFSET + PAYLOAD_LEN_SZ)

#define STOP_SEQ_SZ 4
#define STOP_SEQ_OFFSET (DS_PACKET_SZ - STOP_SEQ_SZ)

#define PAYLOAD_OFFSET (PAYLOAD_TYPE_OFFSET + PAYLOAD_TYPE_SZ)
//#define DATA_SZ  

#define START_SEQ {0x01,0x01,0x44,0x55}
#define STOP_SEQ {0x01,0x01,0x66,0x77}

int ParsePacket(void){
	uint8_t start_seq[] = START_SEQ;
	uint8_t stop_seq[] = STOP_SEQ;

	for (size_t i = 0; i < START_SEQ_SZ; i++){
		if (start_seq[i] != data[i + START_SEQ_OFFSET]){
			return -1;
		}
	}
	
	for (size_t i = 0; i < (STOP_SEQ_SZ); i++){
		if (stop_seq[i] != data[i+STOP_SEQ_OFFSET]){
			return -1;
		}
	}
	
	
	return 0;
}
	
int CheckForPacket(void){
	size_t rv = 0;
	rv = SerialReceive(data, DS_PACKET_SZ);
	if (rv >= DS_PACKET_SZ){
		return 0;
	}
	return -1;
	 
}

typedef enum{
	MQQT,
	REQUEST,
	COMMAND
} PayloadType;

typedef enum{
	ALL_DATA,
	DOOR_DATA,
	OCCUPIED_DATA
} RequestType;

#define REQUEST_TYPE_OFFSET 0
#define REQUEST_DATA_OFFSET 1

int ProcessData(uint8_t* data_p, PayloadType type){
	
	switch(type){
		case MQQT:
			break;
			
		case REQUEST:
			switch(data_p[REQUEST_TYPE_OFFSET]){
				case ALL_DATA:
					data_p[REQUEST_DATA_OFFSET] = 0xFF;
					data_p[REQUEST_DATA_OFFSET+1] = 0xBB;
					memcpy(&data[PAYLOAD_OFFSET],&data_p[REQUEST_DATA_OFFSET], 2);
					SerialSend(data, DS_PACKET_SZ);
					break;
				default:
					assert(0);
					break;
			}
			break;
			
		case COMMAND:
			break;
		
	}
	return 0;
	
}

void program1(void) __attribute__ ((section (".program1")));
void program1(void){
	
	//SetBootImageSelect(PROGRAM2);

	bool temp = GetPinValue(SW_PIN);
	CheckChange(temp);
	while(1){
		
		if (CheckChange(temp)){
			ExecuteStateChange();
		}else{
			_delay_ms(100);
			// check for request packet
			if (CheckForPacket() == 0){
				
				if(ParsePacket() == 0){
					TogglePin(LED1_PIN);
					ProcessData(&data[PAYLOAD_OFFSET], data[PAYLOAD_TYPE_OFFSET]);
// 					data[STOP_SEQ_OFFSET] = '\0'; // terminate string
// 					Print((char*)&data[PAYLOAD_OFFSET]);
 					//Print("\n");					
				}else{
					
					SerialSend(data, DS_PACKET_SZ);
					//data[DS_PACKET_SZ] = '\0'; // terminate string
					//Print((char*)&data[0]);
					//Print("\n");
							
				}
			}
			
			
		}
		
		
		//Print("Hello, I am the Door Sensor!\n");
		sleep();

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

	disable_global_int();
	SerialInit();
	enable_global_int();
	
	SetPinDirection(LED1_PIN, OUTPUT);
	SetPinDirection(SW_PIN, INPUT);
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

