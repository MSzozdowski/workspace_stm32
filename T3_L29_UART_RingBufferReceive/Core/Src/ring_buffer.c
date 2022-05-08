/*
 * ring_buffer.c
 *
 *  Created on: Jan 7, 2022
 *      Author: Michal
 */
#include "main.h"
#include "ring_buffer.h"

RB_Status RB_Read(RingBuffer_t *Buf, uint8_t *Value)
{
	if(Buf->Head == Buf->Tail)
	{
		return RB_ERROR;
	}

	*Value = Buf->Buffer[Buf->Tail];

	Buf->Tail = (Buf->Tail + 1) % RING_BUFFER_SIZE;

	return RB_OK;
}

RB_Status RB_Write(RingBuffer_t *Buf, uint8_t Value)
{
	uint8_t HeadTmp = (Buf->Head + 1) % RING_BUFFER_SIZE;

	if(HeadTmp == Buf->Tail)
	{
		return RB_ERROR;
	}

	Buf->Buffer[Buf->Head] = Value;

	Buf->Head = HeadTmp;


	return RB_OK;
}

void RB_Flush(RingBuffer_t *Buf)
{
	Buf->Head = 0;
	Buf->Tail = 0;
}
