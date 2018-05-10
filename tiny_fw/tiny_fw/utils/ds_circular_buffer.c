/*
 * circular_buffer.c
 *
 * Created: 5/8/2018 1:31:14 PM
 *  Author: amand
 */ 

#include "utils/ds_circular_buffer.h"
#include <stdlib.h>



uint8_t PopCircBuffer(CircularBuffer* buf, uint8_t* data){
	if (buf->tail == buf->head){
		return -1;
	}
	*data = buf->buffer[buf->tail++];
	buf->tail %= buf->buffer_sz;
	return 0;
}

bool PushCircBuffer(CircularBuffer* buf, uint8_t data){
	buf->buffer[buf->head++] = data;
	buf->head %= buf->buffer_sz;
	
	if (buf->head == buf->tail){
		return true;
	}
	return false;
}

size_t GetUsedBytes(CircularBuffer* buf){
	int temp = buf->head - buf->tail;
	return ((temp > 0) ? temp : buf->buffer_sz-temp);
}


