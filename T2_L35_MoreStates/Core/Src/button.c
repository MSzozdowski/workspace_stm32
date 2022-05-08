/*
 * button.c
 *
 *  Created on: Jan 5, 2022
 *      Author: Michal
 */
#include "main.h"

#include "button.h"

void ButtonInitKey(TButton *Key,GPIO_TypeDef *GpioPort, uint16_t GpioPin, uint32_t TimerDebounce, uint32_t TimerLongPress, uint32_t TimerRepeat)
{
	Key->State = IDLE;

	Key->GpioPort = GpioPort;
	Key->GpioPin = GpioPin;

	Key->TimerDebounce = TimerDebounce;
	Key->TimerLongPress = TimerLongPress;
	Key->TimerRepeat = TimerRepeat;
}

void ButtonSetDebounceTime(TButton *Key, uint32_t Milliseconds)
{
	Key->TimerDebounce = Milliseconds;
}

void ButtonSetLongPressTime(TButton *Key, uint32_t Milliseconds)
{
	Key->TimerDebounce = Milliseconds;
}

void ButtonSetRepeatTime(TButton *Key, uint32_t Milliseconds)
{
	Key->TimerDebounce = Milliseconds;
}

void ButtonRegisterPressCallBack(TButton *Key, void *Callback)
{
	Key->ButtonPressed = Callback;
}

void ButtonRegisterLongPressCallBack(TButton *Key, void *Callback)
{
	Key->ButtonLongPressed = Callback;
}

void ButtonRegisterRepeatCallBack(TButton *Key, void *Callback)
{
	Key->ButtonRepeat = Callback;
}

void ButtonRegisterReleaseCallBack(TButton *Key, void *Callback)
{
	Key->ButtonRelease = Callback;
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
			Key->LastTick = HAL_GetTick();
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
		Key->State = RELEASE; //IDLE
	}
	else
	{
		if(HAL_GetTick() - Key->LastTick > Key->TimerLongPress)
		{
			Key->State = REPEAT;
			Key->LastTick = HAL_GetTick();
			if(Key->ButtonLongPressed != NULL)
			{
				Key->ButtonLongPressed();
			}
		}
	}
}

void ButtonRepeatRoutine(TButton *Key)
{
	if(HAL_GPIO_ReadPin(Key->GpioPort,Key->GpioPin) == GPIO_PIN_SET)
	{
		Key->State = RELEASE; //IDLE
	}
	else
	{
		if(HAL_GetTick() - Key->LastTick > Key->TimerRepeat)
		{
			Key->LastTick = HAL_GetTick();

			if(Key->ButtonRepeat != NULL)
			{
				Key->ButtonRepeat();
			}
		}
	}
}

void ButtonReleaseRoutine(TButton *Key)
{
	static uint8_t Counter = 0;
	if(HAL_GetTick() - Key->LastTick > 500){
		Key->LastTick = HAL_GetTick();
		if(Key->ButtonRelease != NULL)
		{
			Key->ButtonRelease();
		}
		Counter ++;
	}
	if(Counter >= 10)
	{
		Counter = 0;
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

		case REPEAT:
			ButtonRepeatRoutine(Key);
		break;

		case RELEASE:
			ButtonReleaseRoutine(Key);
		break;
	}
}
