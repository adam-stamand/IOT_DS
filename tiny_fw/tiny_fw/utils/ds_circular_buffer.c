/*
 * circular_buffer.c
 *
 * Created: 5/8/2018 1:31:14 PM
 *  Author: amand
 */ 

#include "utils/ds_circular_buffer.h"




uint8_t _pop_circ_buffer(CircularBuffer* buf, uint8_t* data){
	if (buf->tail == buf->head){
		return -1;
	}
	*data = buf->buffer[buf->tail++];
	buf->tail %= CIRC_MAX_SZ;
	return 0;
}

bool _push_circ_buffer(CircularBuffer* buf, uint8_t data){
	buf->buffer[buf->head++] = data;
	buf->head %= CIRC_MAX_SZ;
	
	if (buf->head == buf->tail){
		return true;
	}
	return false;
}

size_t _get_bytes_used(CircularBuffer* buf){
	int temp = buf->head - buf->tail;
	return ((temp > 0) ? temp : CIRC_MAX_SZ-temp);
}