/*
 * tiny_fw.c
 *
 * Created: 4/22/2018 3:06:08 AM
 * Author : adams
 */ 

#include "ds_conf.h"
#include "services/services.h"
#include "utils/ds_crc.h"
#include "MQTTSNPacket.h"
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>


volatile MQTTSN_topicid topic;
volatile int global_id;

#define START_SEQ {0x01,0xAA}
#define STOP_SEQ {0x54, 0xDD}

#define DS_PACKET_SZ SERIAL_BUFF_SZ


#define START_SEQ_SZ 2
#define START_SEQ_OFFSET 0

#define STOP_SEQ_SZ 2
#define STOP_SEQ_OFFSET (DS_PACKET_SZ - STOP_SEQ_SZ)


#define CRC_OFFSET (STOP_SEQ_OFFSET - 1)
#define CRC_SZ 1

#define LEN_OFFSET START_SEQ_SZ
#define LEN_SZ 1

#define TYPE_OFFSET START_SEQ_SZ + LEN_SZ
#define TYPE_SZ 1

#define PAYLOAD_SZ (DS_PACKET_SZ - STOP_SEQ_SZ - START_SEQ_SZ - CRC_SZ - LEN_SZ - TYPE_SZ)
#define PAYLOAD_OFFSET START_SEQ_SZ + LEN_SZ + TYPE_SZ



typedef enum {
	DS_MQTT_MSG_TYPE,
	DS_PRINT_MSG_TYPE,
} MsgType;


typedef enum{
	DOOR_OPEN,
	DOOR_CLOSED
	
} DoorState;

typedef enum{
	UNOCCUPIED = 0,	
	OCCUPIED = 1,
} OccupationState;

uint8_t packet_buffer[SERIAL_BUFF_SZ] = {0};
uint8_t* payload_buffer = &packet_buffer[PAYLOAD_OFFSET];
volatile DoorState door_state = DOOR_CLOSED;
volatile OccupationState occupied_state = UNOCCUPIED;


ISR(BADISR_vect){
	while(1){
		TogglePin(LED1_PIN);
		_delay_ms(100);
	}
}


ISR(PCINT0_vect){
	if(GetPinValue(SW_PIN)){
		door_state = DOOR_OPEN;
	}else{
		door_state = DOOR_CLOSED;
	}
	//sleep_disable();
}

void Print(char* str){
	SerialSend((uint8_t*)str, strlen(str));
}

void SerialTest(void){
	Print("Hello, I am the Door Sensor!\n");
}



ImageSelect GetBootImageSelect(void){
	ImageSelect sel;
	ReadEEPROM(BOOT_ADDR, (uint8_t*)&sel, sizeof(ImageSelect));
	return sel;
}

void SetBootImageSelect(ImageSelect sel){
	WriteEEPROM(BOOT_ADDR, (uint8_t*)&sel, sizeof(ImageSelect));
}

// TODO check message type for custom type
int ParsePacket(void){
	uint8_t start_seq[] = START_SEQ;
	uint8_t stop_seq[] = STOP_SEQ;

	for (size_t i = 0; i < START_SEQ_SZ; i++){
		if (start_seq[i] != packet_buffer[i + START_SEQ_OFFSET]){
			return -1;
		}
	}
	
	for (size_t i = 0; i < (STOP_SEQ_SZ); i++){
		if (stop_seq[i] != packet_buffer[i+STOP_SEQ_OFFSET]){
			return -1;
		}
	}
	
	uint8_t actual_crc = packet_buffer[CRC_OFFSET];
	uint8_t expected_crc = calc_crc8(&packet_buffer[PAYLOAD_OFFSET], PAYLOAD_SZ);
	if (actual_crc != expected_crc){
		//return -1; //TODO add back crc
	}
	
	
	return 0;
}

int GrabPacket(void){
	size_t rv = 0;

	rv = SerialReceive(packet_buffer, DS_PACKET_SZ);
	if (rv == DS_PACKET_SZ){
		return 0;
	}
	return -1;
}





int RecvPacket(unsigned char* buf, int count){
	uint8_t len = 3;
	while (len--){
		if (GrabPacket() == 0){
			if (ParsePacket() == 0){
				return packet_buffer[LEN_OFFSET];
			}
		}
		_delay_ms(500);
	}
	return -1;
}

int MQTTRecv(unsigned char* buf, int count){
	return RecvPacket(buf, count);
}

int SendPacket(int len, MsgType msg_type){
	uint8_t start_seq[] = START_SEQ;
	uint8_t stop_seq[] = STOP_SEQ;
	uint8_t length = (uint8_t)len;
	uint8_t type = msg_type;
	memcpy(&packet_buffer[START_SEQ_OFFSET], start_seq, START_SEQ_SZ);
	memcpy(&packet_buffer[STOP_SEQ_OFFSET], stop_seq, STOP_SEQ_SZ);
	packet_buffer[LEN_OFFSET] = length;
	packet_buffer[TYPE_OFFSET] = type;
	packet_buffer[CRC_OFFSET] = calc_crc8(payload_buffer, PAYLOAD_SZ);
	SerialSend(packet_buffer, DS_PACKET_SZ);
	return 0;
}

void PacketTest(void){
	uint8_t test_data[] = {'0','1','2','3','4','5','6','7','8','9'};
	memcpy(payload_buffer, test_data, 10);

}

#define MAX_IR_HITS 3
#define MAX_IR_ITER 6
bool CheckIR(void){
	int cnt = 0;
	uint16_t iter = MAX_IR_ITER;
	_delay_ms(4000); // allow PIR to go low after exiting
	while (iter--){
		if (GetPinValue(IR_PIN)){
			cnt++;
			if (cnt >= MAX_IR_HITS){
				return true;
			}
		}	
		_delay_ms(1000);
	}
	return false;
}


#define MAX_LOOP 3
void DSPublish(uint8_t* data, size_t data_len){
	int rc;
	unsigned short topic_id;
	unsigned short submsgid = 0;
	unsigned short msgid = 0;
	unsigned char returncode;
	uint8_t loop_cnt = 0;
	
	uint16_t len = MQTTSNSerialize_publish(payload_buffer, PAYLOAD_SZ, 0, 1, 0, msgid , topic, data, data_len);
	
	do{
		rc = SendPacket(len, DS_MQTT_MSG_TYPE);
		_delay_ms(500);
	}while (MQTTSNPacket_read(payload_buffer, PAYLOAD_SZ, MQTTRecv) != MQTTSN_PUBACK && loop_cnt++ < MAX_LOOP);
	
	rc = MQTTSNDeserialize_puback(&topic_id, &submsgid, &returncode, payload_buffer, PAYLOAD_SZ);
	
	if ( (rc != 1) || (returncode != MQTTSN_RC_ACCEPTED) || (submsgid != msgid ) || topic.data.id != topic_id);
	{
		//len =sprintf(payload_buffer, "Unable to acknowledge publish %d %d %d %d %d %d\n", returncode, submsgid, msgid, rc, topic.data.id, topic_id);
		//rc= SendPacket(len, DS_PRINT_MSG_TYPE);
	}
	topic.data.id = global_id;
}

void ExecuteStateChange(){
	
	switch(door_state){
		case DOOR_CLOSED:
			if (CheckIR()){
				occupied_state = OCCUPIED;
				int len =sprintf(payload_buffer, "cl/oc");
				SendPacket(len, DS_PRINT_MSG_TYPE);
				// do nothing
			}else{
				occupied_state = UNOCCUPIED;
				int len =sprintf(payload_buffer, "cl/un");
				SendPacket(len, DS_PRINT_MSG_TYPE);
				DSPublish(&occupied_state, 1);
			}
			break;
		
		case DOOR_OPEN:
			if (occupied_state == UNOCCUPIED){
				int len =sprintf(payload_buffer, "op/un");
				SendPacket(len, DS_PRINT_MSG_TYPE);
				occupied_state = OCCUPIED;
				DSPublish(&occupied_state, 1);
			} else if (occupied_state == OCCUPIED){
			int len =sprintf(payload_buffer, "op/oc");
			SendPacket(len, DS_PRINT_MSG_TYPE);
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



//void program1(void) __attribute__ ((section (".program1")));
void program1(void){
		
	volatile bool temp = GetPinValue(SW_PIN);
	CheckChange(temp);
	
	while(1){
		sleep_enable();
	
		temp = GetPinValue(SW_PIN);
		if (CheckChange(temp)){
			ExecuteStateChange();
		}
		/*else{
			// check for request packet
			if (GrabPacket() == 0){
				if(ParsePacket() == 0){
					TogglePin(LED1_PIN);
 					SerialSend(packet_buffer, 10);					
				}
			}
		}
		*/

		sleep_cpu();
		//_delay_ms(5000);
	}
}


void SwitchInit(void){
	clear_pcint();
	SetPinDirection(SW_PIN, INPUT);
	SetPinValue(SW_PIN, HIGH); // Activates Pullup
	enable_pcint();
	enable_pcint_mask(SW_PIN);
}


void ConnectToGateway(void){
	int rc;
	unsigned short topicid;
	MQTTSNPacket_connectData options = MQTTSNPacket_connectData_initializer;
	char *topicname = "living_room/doorsensor1";
	int len; 
	int packetid = 1;
	MQTTSNString topicstr;
	topicstr.cstring = topicname;
	topicstr.lenstring.len = strlen(topicname);
	unsigned char returncode;
	unsigned short submsgid;
		
	// Connect with Gateway //
	// Fill payload buffer with connect message
	len = MQTTSNSerialize_connect(payload_buffer, PAYLOAD_SZ, &options);
	options.clientID.cstring = "doorsensor1";
	// Send connect message until connetion ACK received
	do{
		rc = SendPacket(len, DS_MQTT_MSG_TYPE);
		_delay_ms(1000);
		
	}while (MQTTSNPacket_read(payload_buffer, PAYLOAD_SZ, MQTTRecv) != MQTTSN_CONNACK);
	// Check for successful connection
	rc = MQTTSNDeserialize_connack((int*)&returncode, payload_buffer, PAYLOAD_SZ);
	if ( rc != 1 || returncode!= 0)
	{
		//len =sprintf(payload_buffer, "Unable to connect, return code %d, rc %d\n", returncode, rc);
		//rc = SendPacket(len, DS_PRINT_MSG_TYPE);
	}

	// Register with Gateway //
	// Fill payload buffer with register message
	len = MQTTSNSerialize_register(payload_buffer, PAYLOAD_SZ, 0, packetid, &topicstr);
	// Send connect message until registration ACK received
	do{
		rc = SendPacket(len, DS_MQTT_MSG_TYPE);
		_delay_ms(1000);
			
	}while (MQTTSNPacket_read(payload_buffer, PAYLOAD_SZ, MQTTRecv) != MQTTSN_REGACK);
	
	rc = MQTTSNDeserialize_regack(&topicid, &submsgid, &returncode, payload_buffer, PAYLOAD_SZ);
	if ( (rc != 1) || (returncode != 0) || (submsgid != packetid));
	{
		//len =sprintf(payload_buffer, "Unable to register, return code %d, msgid %d, rc %d\n", returncode, submsgid, rc);
		//rc= SendPacket(len, DS_PRINT_MSG_TYPE);
	}
	
	topic.type = MQTTSN_TOPIC_TYPE_NORMAL;
	global_id = topicid;
	topic.data.id = global_id;
}


void UpdateFirmware(void){
	// Check for firmware update
	int rv = RecvPacket(packet_buffer, 0);
	if (rv != -1){
		if (ParsePacket() ==0){
			// update firmware
		}
	}	
}

//TODO add watchdog
int main(void)
{
	// Begin Init
	disable_global_int();
	disable_pcint();
	SerialInit();
	sleep_enable(); //TODO make sure this is included where it's needed; been causing issues
	enable_global_int();
	ConnectToGateway();

	// Update firmware
	UpdateFirmware();
	
	// Complete Init
	SwitchInit();
	SetPinDirection(LED1_PIN, OUTPUT);
	SetPinDirection(IR_PIN, INPUT);	
	
    ImageSelect sel = GetBootImageSelect();
	sel = PROGRAM1;
	
	switch(sel){
		
		case PROGRAM1:
			program1();
			break;
		
		case PROGRAM2:
			//program2();
			break;
		
		default:
			assert(0);
		
	}
	
}

