/*
 * utils.c
 *
 *  Created on: Jan 8, 2022
 *      Author: Michal
 */
#include "main.h"
#include "string.h"
#include "usart.h"
void UartLog(char* Message)
{
	HAL_UART_Transmit(&huart2, (uint8_t*)Message, strlen(Message), 1000);
}
