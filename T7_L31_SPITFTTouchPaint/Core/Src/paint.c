/*
 * paint.c
 *
 *  Created on: Aug 21, 2022
 *      Author: Michal
 */
#include "main.h"
#include "paint.h"
#include "TFT_ILI9341.h"
#include "XPT2046.h"
#include "GFX_Color.h"
#include "GFX_EnhancedFonts.h"

#define DRAW_WINDOW_X_START 0
#define DRAW_WINDOW_X_STOP ILI9341_TFTWIDTH
#define DRAW_WINDOW_Y_START 30
#define DRAW_WINDOW_Y_STOP (ILI9341_TFTHEIGHT-60)

#define CLEAR_BUTTON_X ILI9341_TFTWIDTH-130
#define CLEAR_BUTTON_Y 3
#define CLEAR_BUTTON_WIDTH 120
#define CLEAR_BUTTON_HEIGHT 26

#define COLOR_BUTTON_WIDTH 36
#define COLOR_BUTTON_HEIGHT 20

#define USED_COLORS	5

typedef enum
{
	PAINT_INIT, // Build GUI
	PAINT_DRAW, // Read Touch and draw pixels
	PAINT_CLEAR // Clear drawing area
} PaintState;

PaintState State = PAINT_INIT;
uint16_t CurrentColor = ILI9341_BLACK;
uint16_t UsedColors[USED_COLORS] = {ILI9341_BLACK, ILI9341_BLUE, ILI9341_GREEN, ILI9341_RED, ILI9341_ORANGE};

void ColorIndicator(void)
{
	GFX_DrawFillCircle(ILI9341_TFTWIDTH/2, 15, 12, CurrentColor); // Drawing circle with GFX Library
}

void ClearButton(void)
{
	GFX_DrawFillRectangle(CLEAR_BUTTON_X, CLEAR_BUTTON_Y, CLEAR_BUTTON_WIDTH, CLEAR_BUTTON_HEIGHT, ILI9341_DARKGREEN); // Button Color
	  //ILI9341_ClearArea(CLEAR_BUTTON_X, CLEAR_BUTTON_Y, CLEAR_BUTTON_WIDTH, CLEAR_BUTTON_HEIGHT, ILI9341_DARKGREEN); // Homework - to write this function in TFT library
	EF_PutString((const uint8_t*)"CZYŚĆ", CLEAR_BUTTON_X+5, CLEAR_BUTTON_Y, ILI9341_WHITE, BG_TRANSPARENT, ILI9341_DARKGREEN); // Button text
}

void ColorButtons(void)
{
	uint8_t i;
	for(i = 1; i <= USED_COLORS; i++) // For each color
	{
		GFX_DrawFillRectangle((i*(ILI9341_TFTWIDTH/(USED_COLORS+1)))-(COLOR_BUTTON_WIDTH/2), ILI9341_TFTHEIGHT-25, COLOR_BUTTON_WIDTH, COLOR_BUTTON_HEIGHT, UsedColors[i-1]);
		// X calculation: (i*(ILI9341_TFTWIDTH/(USED_COLORS+1)))-(COLOR_BUTTON_WIDTH/2)
		//					(i*(ILI9341_TFTWIDTH/(USED_COLORS+1))) - define a middle of each button. That's why (USED_COLORS+1)
		//					-(COLOR_BUTTON_WIDTH/2) - Move back by half of button.

		//ILI9341_ClearArea((i*(ILI9341_TFTWIDTH/(USED_COLORS+1)))-(BUTTON_WIDTH/2), ILI9341_TFTHEIGHT-25, BUTTON_WIDTH, BUTTON_HEIGHT, UsedColors[i-1]);
	}
}

uint8_t IsColorButtonTouched(uint16_t x, uint16_t y)
{
	uint8_t i;
	for(i = 1; i <= USED_COLORS; i++) // For each color used
	{
		 // Check if Touch point is higher than X begin of current color button
		if(x > (i*(ILI9341_TFTWIDTH/(USED_COLORS+1)))-(COLOR_BUTTON_WIDTH/2))
		{
			 // Check if Touch point is lower than X end of current color button
			if(x < (i*(ILI9341_TFTWIDTH/(USED_COLORS+1)))+(COLOR_BUTTON_WIDTH/2))
			{
				// Check if Touch point is higher than Y begin of current color button
				if(y > ILI9341_TFTHEIGHT-25)
				{
					// Check if Touch point is lower than Y end of current color button
					if(y < (ILI9341_TFTHEIGHT-25+COLOR_BUTTON_HEIGHT))
					{
						// If we are sure that touched point was inside current (i) button - return button number
						return i;
					}
				}
			}
		}
	}
	// If no color button touched
	return 0;
}


uint8_t IsClearButtonTouched(uint16_t x, uint16_t y)
{
	// Check if Touch point is higher than X begin of clear button
	if(x > CLEAR_BUTTON_X)
	{
		// Check if Touch point is higher than X end of clear button
		if(x < (CLEAR_BUTTON_X + CLEAR_BUTTON_WIDTH))
		{
			// Check if Touch point is higher than Y begin of clear button
			if(y > CLEAR_BUTTON_Y)
			{
				// Check if Touch point is higher than Y end of clear button
				if(y < (CLEAR_BUTTON_Y+CLEAR_BUTTON_HEIGHT))
				{
					// If we are sure that touched point was inside clear button - return 1
					return 1;
				}
			}
		}
	}
	// If clear button is not touched
	return 0;
}


void InitScreen(void)
{
	ILI9341_ClearDisplay(ILI9341_WHITE);
	EF_PutString((const uint8_t*)"Paint", 5, 2, ILI9341_BLACK, BG_TRANSPARENT, ILI9341_GREEN);
	GFX_DrawRectangle(DRAW_WINDOW_X_START, DRAW_WINDOW_Y_START, DRAW_WINDOW_X_STOP, DRAW_WINDOW_Y_STOP, ILI9341_BLACK);
	ColorIndicator();
	ClearButton();
	ColorButtons();
	State = PAINT_DRAW;
}

void DrawScreen(void)
{
	  if(XPT2046_IsTouched())
	  {
		  uint16_t x,y;
		  uint8_t ColorButtonNubmer;
		  XPT2046_GetTouchPoint(&x, &y);
		  if((x > DRAW_WINDOW_X_START) && (x < DRAW_WINDOW_X_STOP) && (y > DRAW_WINDOW_Y_START) && (y < DRAW_WINDOW_Y_STOP+30))
		  {
			  //GFX_DrawPixel(x, y, CurrentColor);
			  GFX_DrawFillCircle(x,y,2,CurrentColor);
		  }

		  ColorButtonNubmer = IsColorButtonTouched(x, y);
		  if(ColorButtonNubmer != 0) // If yes
		  {
			  CurrentColor = UsedColors[ColorButtonNubmer-1];
			  ColorIndicator();
		  }

		  if(IsClearButtonTouched(x, y))
		  {
			  // Jump to Clearing state
			  State = PAINT_CLEAR;
		  }
	  }
}

void DrawClear(void)
{
	GFX_DrawFillRectangle(0, 30, ILI9341_TFTWIDTH, ILI9341_TFTHEIGHT-60, ILI9341_WHITE);
	//ILI9341_ClearArea(0, 30, ILI9341_TFTWIDTH, ILI9341_TFTHEIGHT-60, ILI9341_WHITE);
	GFX_DrawRectangle(0, 30, ILI9341_TFTWIDTH, ILI9341_TFTHEIGHT-60, ILI9341_BLACK);
	State = PAINT_DRAW;
}

void Paint(void)
{
	switch(State)
	{
	case PAINT_INIT:
		InitScreen();
		break;
	case PAINT_DRAW:
		DrawScreen();
		  break;
	case PAINT_CLEAR:
		DrawClear();
		break;
	}

}
