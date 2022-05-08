/*
 * button.h
 *
 *  Created on: Jan 5, 2022
 *      Author: Michal
 */

#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

typedef enum
{
	IDLE = 0,
	DEBOUNCE,
	PRESSED
} BUTTON_STATE;

typedef struct
{
	BUTTON_STATE 	State;

	GPIO_TypeDef 	*GpioPort;
	uint16_t 		GpioPin;

	uint32_t 		LastTick;
	uint32_t 		TimerDebounce; //Fixed

	void(*ButtonPressed)(void);
} TButton;

void ButtonInitKey(TButton *Key,GPIO_TypeDef *GpioPort, uint16_t GpioPin, uint32_t TimerDebounce);

void ButtonTask(TButton *Key);

void ButtonRegisterPressCallBack(TButton *Key, void *Callback);
#endif /* INC_BUTTON_H_ */
