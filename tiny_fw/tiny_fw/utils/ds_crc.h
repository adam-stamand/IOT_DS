/*
 * ds_crc.h
 *
 * Created: 5/19/2018 3:12:35 PM
 *  Author: adams
 */ 


#ifndef DS_CRC_H_
#define DS_CRC_H_


#include <stdint.h>
#include <stdlib.h>


#define CRC8_INITIAL 0x00
#define CRC8_FINAL 0x00
#define CRC8_POLY 0x07

uint8_t calc_crc8(uint8_t* data, size_t len);

#endif /* DS_CRC_H_ */