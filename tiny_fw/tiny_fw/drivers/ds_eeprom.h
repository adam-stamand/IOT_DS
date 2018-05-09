/*
 * ds_eeprom.h
 *
 * Created: 5/8/2018 2:06:45 PM
 *  Author: amand
 */ 


#ifndef DS_EEPROM_H_
#define DS_EEPROM_H_

#include <avr/io.h>

inline void set_eeprom_mode(uint8_t mode){
	EECR = (0<<EEPM1)|(0<<EEPM0);
}

inline void enable_master_eeprom(void){
	EECR |= (1<<EEMPE);
}

inline void enable_program_eeprom(void){
	EECR |= (1<<EEPE);
}

inline void enable_read_eeprom(void){
	EECR |= (1<<EERE);
}

inline uint8_t read_program_eeprom(void){
	return (EECR & (1 << EEPE));
}

inline void set_data_eeprom(uint8_t data){
	EEDR = data;
}

inline void set_address_eeprom(uint16_t addr){
	EEAR = addr;
}

inline uint8_t get_data_eeprom(){
	return EEDR;
}

inline uint8_t read_byte_eeprom(uint16_t addr){
	set_address_eeprom(addr);
	enable_master_eeprom();
	enable_read_eeprom();
	return get_data_eeprom();
}


inline void write_byte_eeprom(uint16_t addr, uint8_t data){
	set_address_eeprom(addr);
	set_data_eeprom(data);
	enable_master_eeprom();
	enable_program_eeprom();
}


#endif /* DS_EEPROM_H_ */