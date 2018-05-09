/*
 * circular_buffer.h
 *
 * Created: 5/8/2018 4:02:14 PM
 *  Author: amand
 */ 


#ifndef CIRCULAR_BUFFER_H_
#define CIRCULAR_BUFFER_H_


#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define CIRC_MAX_SZ 100

typedef struct _CircularBuffer{
	uint16_t head;
	uint16_t tail;
	uint8_t buffer[CIRC_MAX_SZ];
} CircularBuffer;

uint8_t _pop_circ_buffer(CircularBuffer* buf, uint8_t* data);
bool _push_circ_buffer(CircularBuffer* buf, uint8_t data);
size_t _get_bytes_used(CircularBuffer* buf);




#endif /* CIRCULAR_BUFFER_H_ */