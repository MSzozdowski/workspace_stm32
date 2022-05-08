/*
 * parser_simple.c
 *
 *  Created on: Jan 8, 2022
 *      Author: Michal
 */

#include "main.h"
#include "ring_buffer.h"
#include "parser_simple.h"
#include "string.h"
#include "utils.h"

void Parser_TakeLine(RingBuffer_t *Buf, uint8_t *Destination)
{
	uint8_t Tmp;
	uint8_t i = 0;

	do
	{
	  RB_Read(Buf, &Tmp);
	  if(Tmp == ENDLINE)
	  {
		  Destination[i] = 0;
	  }
	  else
	  {
		  Destination[i] = Tmp;
	  }
	  i++;
	}while(Tmp != ENDLINE);

}

void Parser_Parse(uint8_t *DataToParse)
{
	if(strcmp("LED_ON",(char*) DataToParse) == 0)
	{
		HAL_GPIO_WritePin(LD2_GPIO_Port,LD2_Pin,SET);
		UartLog("LED_ON\n\r");
	}
	else if (strcmp("LED_OFF",(char*) DataToParse) == 0)
	{
		HAL_GPIO_WritePin(LD2_GPIO_Port,LD2_Pin,RESET);
		UartLog("LED_OFF\n\r");
	}
}
