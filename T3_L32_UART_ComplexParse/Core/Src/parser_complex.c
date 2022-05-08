/*
 * parser_simple.c
 *
 *  Created on: Jan 8, 2022
 *      Author: Michal
 */


#include "main.h"
#include "utils.h"
#include "ring_buffer.h"
#include "parser_complex.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

#define MAX_NAME_SIZE 32

static char MyName[MAX_NAME_SIZE] = "No Name";

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

static void Parser_ParseLED(void)
{
	char *ParsePointer = strtok(NULL,",");
	if(strlen(ParsePointer)>0)
	{
		if(ParsePointer[0] < '0' || ParsePointer[0] > '1')
		{
			UartLog("LED wrong value. Please type 0 or 1! \n\r");
			return;
		}

		if(ParsePointer[0] == '1')
		{
			HAL_GPIO_WritePin(LD2_GPIO_Port,LD2_Pin,SET);
			UartLog("LED on \n\r");
		}
		else if (ParsePointer[0] == '0')
		{
			HAL_GPIO_WritePin(LD2_GPIO_Port,LD2_Pin,RESET);
			UartLog("LED off \n\r");
		}
	}
}

static void Parser_ParseENV(void)
{
	float EnvParameters[3];
	char Message[32];

	for(uint8_t i = 0; i < 3; i++)
	{
		char *ParsePointer = strtok(NULL,",");
		if(strlen(ParsePointer) > 0)
		{
			for(uint8_t j = 0; ParsePointer[j] != 0; j++)
			{
				if((ParsePointer[j] < '0' && ParsePointer[j] >'9') && ParsePointer[j] != '.')
				{
					UartLog("ENV wrong value. Don't use letters dude! \n\r");
					return;
				}
			}
			EnvParameters[i] = atof(ParsePointer);
		}
		else
		{
			UartLog("ENV too less values. ENV=X,Y,Z \n\r");
			return;
		}
	}
	sprintf(Message, "Temperature: %.1f \n\r", EnvParameters[0]);
	UartLog(Message);

	sprintf(Message, "Humidity: %.1f \n\r", EnvParameters[1]);
	UartLog(Message);

	sprintf(Message, "Pressure: %.1f \n\r", EnvParameters[2]);
	UartLog(Message);
}

static void Parser_ParseNAME(void)
{
	char *ParsePointer = strtok(NULL,",");
	char Message[MAX_NAME_SIZE+32];

	if(strlen(ParsePointer) > 0)
	{
		if(strcmp("?" , ParsePointer) == 0)
		{
			sprintf(Message, "My name is %s \r\n", MyName);
			UartLog(Message);
		}
		else
		{
			if(strlen(ParsePointer) > MAX_NAME_SIZE)
			{
				UartLog("Name should be less than 32 \r\n");
				return;
			}
			strcpy(MyName,ParsePointer);
			sprintf(Message, "My new name is %s \n\r", MyName);
			UartLog(Message);
		}
	}
	else
	{
		UartLog("Name can't be empty \r\n");
	}

}

void Parser_Parse(uint8_t *DataToParse)
{
	char *ParsePointer = strtok((char*)DataToParse,"=");

	if(strcmp("LED", ParsePointer) == 0)
	{
		Parser_ParseLED();
	}
	else if(strcmp("ENV", ParsePointer) == 0)
	{
		Parser_ParseENV();
	}
	else if(strcmp("NAME", ParsePointer) == 0)
	{
		Parser_ParseNAME();
	}
}
