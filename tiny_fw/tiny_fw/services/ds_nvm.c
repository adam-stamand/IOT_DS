/*
 * ds_nvm.c
 *
 * Created: 5/8/2018 2:05:36 PM
 *  Author: amand
 */ 


#include "services/ds_nvm.h"
#include "drivers/drivers.h"


//TODO add timeout
void WriteEEPROM(uint16_t addr, uint8_t * data, size_t len){
	while (len--){
		while(read_program_eeprom());
		write_byte_eeprom(addr, *data++);
	}
}



//TODO add timeout
void ReadEEPROM(uint16_t addr, uint8_t * data, size_t len){
	while (len--){
		while(read_program_eeprom());
		*data++ = read_byte_eeprom(addr);
	}
}