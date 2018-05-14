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



typedef struct _CircularBuffer{
	size_t head;
	size_t tail;
	size_t buffer_sz;
	uint8_t *buffer;
} CircularBuffer;

uint8_t PopCircBuffer(CircularBuffer* buf, uint8_t* data);
bool PushCircBuffer(CircularBuffer* buf, uint8_t data);
size_t GetUsedBytes(CircularBuffer* buf);
void FlushBuffer(CircularBuffer * buf);



#endif /* CIRCULAR_BUFFER_H_ */