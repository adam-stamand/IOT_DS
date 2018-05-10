/*
 * ds_nvm.h
 *
 * Created: 5/8/2018 2:10:16 PM
 *  Author: amand
 */ 


#ifndef DS_NVM_H_
#define DS_NVM_H_

#include <stdint.h>
#include <stddef.h>
#include "drivers/drivers.h"


#define EEPROM_TIMEOUT 1000


inline int WriteEEPROM(uint16_t addr, uint8_t * data, size_t len){
	size_t timeout = 0;
	while (len--){
		while(read_program_eeprom()){
			timeout++;
			if (timeout > EEPROM_TIMEOUT){
				return -1;
			}
		}
		write_byte_eeprom(addr, *data++);
	}
	return 0;
}

inline int ReadEEPROM(uint16_t addr, uint8_t * data, size_t len){
	size_t timeout = 0;
	while (len--){
		while(read_program_eeprom()){
			timeout++;
			if (timeout > EEPROM_TIMEOUT){
				return -1;
			}
		}
		*data++ = read_byte_eeprom(addr);
	}
	return 0;
}

#endif /* DS_NVM_H_ */