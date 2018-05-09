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

void WriteEEPROM(uint16_t addr, uint8_t * data, size_t len);

void ReadEEPROM(uint16_t addr, uint8_t * data, size_t len);



#endif /* DS_NVM_H_ */