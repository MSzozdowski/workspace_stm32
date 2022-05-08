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
	PRESSED,
	REPEAT,
	RELEASE
} BUTTON_STATE;

typedef struct
{
	BUTTON_STATE 	State;

	GPIO_TypeDef 	*GpioPort;
	uint16_t 		GpioPin;

	uint32_t 		LastTick;
	uint32_t 		TimerDebounce;
	uint32_t 		TimerLongPress;
	uint32_t 		TimerRepeat;

	void(*ButtonPressed)(void);
	void(*ButtonLongPressed)(void);
	void(*ButtonRepeat)(void);
	void(*ButtonRelease)(void);
} TButton;

void ButtonInitKey(TButton *Key,GPIO_TypeDef *GpioPort, uint16_t GpioPin, uint32_t TimerDebounce, uint32_t TimerLongPress, uint32_t TimerRepeat);

void ButtonSetDebounceTime(TButton *Key, uint32_t Milliseconds);

void ButtonSetLongPressTime(TButton *Key, uint32_t Milliseconds);

void ButtonSetRepeatTime(TButton *Key, uint32_t Milliseconds);

void ButtonTask(TButton *Key);

void ButtonRegisterPressCallBack(TButton *Key, void *Callback);

void ButtonRegisterLongPressCallBack(TButton *Key, void *Callback);

void ButtonRegisterRepeatCallBack(TButton *Key, void *Callback);

void ButtonRegisterReleaseCallBack(TButton *Key, void *Callback);
#endif /* INC_BUTTON_H_ */
