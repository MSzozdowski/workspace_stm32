/*
 * button.c
 *
 *  Created on: Jan 5, 2022
 *      Author: Michal
 */
#include "main.h"

#include "button.h"

void ButtonInitKey(TButton *Key,GPIO_TypeDef *GpioPort, uint16_t GpioPin, uint32_t TimerDebounce)
{
	Key->State = IDLE;

	Key->GpioPort = GpioPort;
	Key->GpioPin = GpioPin;

	Key->TimerDebounce = TimerDebounce;
}

void ButtonRegisterPressCallBack(TButton *Key, void *Callback)
{
	Key->ButtonPressed = Callback;
}

void ButtonIdleRoutine(TButton *Key)
{
	if(HAL_GPIO_ReadPin(Key->GpioPort,Key->GpioPin) == GPIO_PIN_RESET)
	{
		Key->State = DEBOUNCE;
		Key->LastTick = HAL_GetTick();
	}
}

void ButtonDebounceRoutine(TButton *Key)
{
	if(HAL_GetTick() - Key->LastTick > Key->TimerDebounce)
	{
		if(HAL_GPIO_ReadPin(Key->GpioPort,Key->GpioPin) == GPIO_PIN_RESET)
		{
			Key->State = PRESSED;
			if(Key->ButtonPressed != NULL)
			{
				Key->ButtonPressed();
			}
		}
		else
		{
			Key->State = IDLE;
		}
	}
}

void ButtonPressedRoutine(TButton *Key)
{
	if(HAL_GPIO_ReadPin(Key->GpioPort,Key->GpioPin) == GPIO_PIN_SET)
	{
		Key->State = IDLE;
	}
}

void ButtonTask(TButton *Key)
{
	switch(Key->State)
	{
		case IDLE:
			ButtonIdleRoutine(Key);
		break;

		case DEBOUNCE:
			ButtonDebounceRoutine(Key);
		break;

		case PRESSED:
			ButtonPressedRoutine(Key);
		break;
	}
}
