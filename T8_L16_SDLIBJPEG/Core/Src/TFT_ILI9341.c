#include "main.h"
#include "TFT_ILI9341.h"

//
// Rotation stuff
//
#define MADCTL_MY 0x80  ///< Bottom to top
#define MADCTL_MX 0x40  ///< Right to left
#define MADCTL_MV 0x20  ///< Reverse Mode
#define MADCTL_ML 0x10  ///< LCD refresh Bottom to top
#define MADCTL_RGB 0x00 ///< Red-Green-Blue pixel order
#define MADCTL_BGR 0x08 ///< Blue-Green-Red pixel order
#define MADCTL_MH 0x04  ///< LCD refresh right to left

SPI_HandleTypeDef *Tft_hspi; // SPI Handler

#if(ILI9341_USE_SD_CARD == 1)
#if(ILI9341_USE_JPEG == 1)


#include "jdata_conf.h"
#include "jpeglib.h"

/* This struct contains the JPEG decompression parameters */
struct jpeg_decompress_struct cinfo;
/* This struct represents a JPEG error handler */
struct jpeg_error_mgr jerr;

uint8_t _aucLine[ILI9341_TFTWIDTH*3];

typedef struct RGB
{
	uint8_t R;
	uint8_t G;
	uint8_t B;
}RGB_typedef;
RGB_typedef *RGB_matrix;
#endif
#endif
//
//  Basic function - write those for your MCU
//
static void ILI9341_Delay(uint32_t ms)
{
	HAL_Delay(ms);
}

static void ILI9341_SendToTFT(uint8_t *Byte, uint32_t Length)
{
#if (ILI9341_OPTIMIZE_HAL_SP1 == 1)
	// That is taken from HAL Transmit function
    while (Length > 0U)
    {
      /* Wait until TXE flag is set to send data */
      if(__HAL_SPI_GET_FLAG(Tft_hspi, SPI_FLAG_TXE))
      {
    	//Fill Data Register in SPI
        *((__IO uint8_t *)&Tft_hspi->Instance->DR) = (*Byte);
        // Next byte
        Byte++;
        // Length decrement
        Length--;
      }
    }

    // Wait for Transfer end
	while(__HAL_SPI_GET_FLAG(Tft_hspi, SPI_FLAG_BSY) != RESET)
	{

	}
#else
	HAL_SPI_Transmit(Tft_hspi, Byte, Length, ILI9341_SPI_TIMEOUT); 	// Send the command byte
#endif
}

static void ILI9341_SendCommand(uint8_t Command)
{
	// CS Low
#if (ILI9341_USE_CS == 1)
	ILI9341_CS_LOW;
#endif
	// DC to Command - DC to Low
	ILI9341_DC_LOW;

	// Send to TFT 1 byte
	ILI9341_SendToTFT(&Command, 1);

	// CS High
#if (ILI9341_USE_CS == 1)
	ILI9341_CS_HIGH;
#endif
}

static void ILI9341_SendCommandAndData(uint8_t Command, uint8_t *Data, uint32_t Length)
{
	// CS Low
#if (ILI9341_USE_CS == 1)
	ILI9341_CS_LOW;
#endif
	// DC to Command - DC to Low
	ILI9341_DC_LOW;
	// Send to TFT 1 byte
	ILI9341_SendToTFT(&Command, 1);

	// DC to Data - DC to High
	ILI9341_DC_HIGH;
	// Send to TFT Length byte
	ILI9341_SendToTFT(Data, Length);

	// CS High
#if (ILI9341_USE_CS == 1)
	ILI9341_CS_HIGH;
#endif
}
#if (ILI9341_OPTIMIZE_HAL_SP1 == 0)
static void ILI9341_SendData16(uint16_t Data)
{
#if (ILI9341_USE_CS == 1)
	ILI9341_CS_LOW;
#endif

	uint8_t tmp[2];
	tmp[0] = (Data>>8);
	tmp[1] = Data & 0xFF;

	ILI9341_DC_HIGH;	// Data mode
	ILI9341_SendToTFT(tmp, 2);

#if (ILI9341_USE_CS == 1)
	ILI9341_CS_HIGH;
#endif
}
#endif

//
// TFT Functions
//
void ILI9341_SetRotation(uint8_t Rotation)
{
	if(Rotation > 3)
		return;

	//
	// Set appropriate bits for Rotation
	//
	switch(Rotation)
	{
	case 0:
		Rotation = (MADCTL_MX | MADCTL_BGR);
		break;
	case 1:
		Rotation = (MADCTL_MV | MADCTL_BGR);
		break;
	case 2:
		Rotation = (MADCTL_MY | MADCTL_BGR);
		break;
	case 3:
		Rotation = (MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR);
		break;
	}

	// Write indo MAD Control register our Rotation data
	ILI9341_SendCommandAndData(ILI9341_MADCTL, &Rotation, 1);
}

void ILI9341_SetAddrWindow(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h)
{
	uint8_t DataToTransfer[4];
	// Calculate end ranges
	uint16_t x2 = (x1 + w - 1), y2 = (y1 + h - 1);

	// Fulfill X's buffer
	DataToTransfer[0] = x1 >> 8;
	DataToTransfer[1] = x1 & 0xFF;
	DataToTransfer[2] = x2 >> 8;
	DataToTransfer[3] = x2 & 0xFF;
	// Push X's buffer
	ILI9341_SendCommandAndData(ILI9341_CASET, DataToTransfer, 4);

	// Fulfill Y's buffer
	DataToTransfer[0] = y1 >> 8;
	DataToTransfer[1] = y1 & 0xFF;
	DataToTransfer[2] = y2 >> 8;
	DataToTransfer[3] = y2 & 0xFF;
	// Push Y's buffer
	ILI9341_SendCommandAndData(ILI9341_PASET, DataToTransfer, 4);
}

void ILI9341_WritePixel(int16_t x, int16_t y, uint16_t color)
{
	uint8_t DataToTransfer[2];

	if ((x >= 0) && (x < ILI9341_TFTWIDTH) && (y >= 0) && (y < ILI9341_TFTHEIGHT))
	{
		// Set Window for 1x1 pixel
		ILI9341_SetAddrWindow(x, y, 1, 1);

		// Fulfill buffer with color
		DataToTransfer[0] = color >> 8;
		DataToTransfer[1] = color & 0xFF;
		// Push color bytes to RAM
		ILI9341_SendCommandAndData(ILI9341_RAMWR, DataToTransfer, 2);
	}
}

void ILI9341_DrawImage(uint16_t x, uint16_t y, const uint8_t *img, uint16_t w, uint16_t h)
{
	// Check if image will fit into screen - cannot make it outside by hardware
	if ((x >= 0) && ((x + w) <= ILI9341_TFTWIDTH) && (y >= 0) && ((y + h) <= ILI9341_TFTHEIGHT))
	{
		// Set window for image
		ILI9341_SetAddrWindow(x, y, w, h);
		// Push image to RAM
		ILI9341_SendCommandAndData(ILI9341_RAMWR, (uint8_t *)img, (w*h*2));
	}
}

void ILI9341_ClearDisplay(uint16_t Color)
{
	// Set window for whole screen
	ILI9341_SetAddrWindow(0, 0, ILI9341_TFTWIDTH, ILI9341_TFTHEIGHT);
	// Set RAM writing
	ILI9341_SendCommand(ILI9341_RAMWR);

#if (ILI9341_OPTIMIZE_HAL_SP1 == 1)
	uint32_t Length = ILI9341_TFTWIDTH * ILI9341_TFTHEIGHT;

#if (ILI9341_USE_CS == 1)
	ILI9341_CS_LOW;
#endif
	ILI9341_DC_HIGH;	// Data mode

    while (Length > 0U)
    {
      /* Wait until TXE flag is set to send data */
      if(__HAL_SPI_GET_FLAG(Tft_hspi, SPI_FLAG_TXE))
      {
    	  // Write higher byte of color to DR
        *((__IO uint8_t *)&Tft_hspi->Instance->DR) = (Color >> 8);
        // Wait for transfer
        while(__HAL_SPI_GET_FLAG(Tft_hspi, SPI_FLAG_TXE) != SET)
        {}
        // Write lower byt of color to DR
        *((__IO uint8_t *)&Tft_hspi->Instance->DR) = (Color & 0xFF);
        // Decrease Lenght
        Length--;
      }
    }

    // Wait for the end of transfer
	while(__HAL_SPI_GET_FLAG(Tft_hspi, SPI_FLAG_BSY) != RESET)
	{

	}

#if (ILI9341_USE_CS == 1)
	ILI9341_CS_HIGH;
#endif
#else
	for(uint32_t i = 0; i < (ILI9341_TFTWIDTH * ILI9341_TFTHEIGHT); i++)
	{
		// Send Color bytes
		ILI9341_SendData16(Color);
	}
#endif
}


static const uint8_t initcmd[] = {
  0xEF, 3, 0x03, 0x80, 0x02,
  0xCF, 3, 0x00, 0xC1, 0x30,
  0xED, 4, 0x64, 0x03, 0x12, 0x81,
  0xE8, 3, 0x85, 0x00, 0x78,
  0xCB, 5, 0x39, 0x2C, 0x00, 0x34, 0x02,
  0xF7, 1, 0x20,
  0xEA, 2, 0x00, 0x00,
  ILI9341_PWCTR1  , 1, 0x23,             // Power control VRH[5:0]
  ILI9341_PWCTR2  , 1, 0x10,             // Power control SAP[2:0];BT[3:0]
  ILI9341_VMCTR1  , 2, 0x3e, 0x28,       // VCM control
  ILI9341_VMCTR2  , 1, 0x86,             // VCM control2
  ILI9341_MADCTL  , 1, 0x48,             // Memory Access Control
  ILI9341_VSCRSADD, 1, 0x00,             // Vertical scroll zero
  ILI9341_PIXFMT  , 1, 0x55,
  ILI9341_FRMCTR1 , 2, 0x00, 0x18,
  ILI9341_DFUNCTR , 3, 0x08, 0x82, 0x27, // Display Function Control
  0xF2, 1, 0x00,                         // 3Gamma Function Disable
  ILI9341_GAMMASET , 1, 0x01,             // Gamma curve selected
  ILI9341_GMCTRP1 , 15, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, // Set Gamma
    0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00,
  ILI9341_GMCTRN1 , 15, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, // Set Gamma
    0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,
  ILI9341_SLPOUT  , 0x80,                // Exit Sleep
  ILI9341_DISPON  , 0x80,                // Display on
  0x00                                   // End of list
};

void ILI9341_Init(SPI_HandleTypeDef *hspi)
{
	Tft_hspi = hspi;

    uint8_t cmd, x, numArgs;
    const uint8_t *addr = initcmd;
#if (ILI9341_OPTIMIZE_HAL_SP1 == 1)
    __HAL_SPI_ENABLE(Tft_hspi);
#endif

#if (ILI9341_USE_HW_RESET == 1)
	ILI9341_RST_LOW;
	ILI9341_Delay(10);
	ILI9341_RST_HIGH;
	ILI9341_Delay(10);
#else
	ILI9341_SendCommand(ILI9341_SWRESET); // Engage software reset
    ILI9341_Delay(150);
#endif

    while ((cmd = *(addr++)) > 0)
    {
      x = *(addr++);
      numArgs = x & 0x7F;
      // Push Init data
      ILI9341_SendCommandAndData(cmd, (uint8_t *)addr, numArgs);

      addr += numArgs;

      if (x & 0x80)
      {
    	  ILI9341_Delay(150);
      }
    }

    // Set selected Rotation
    ILI9341_SetRotation(ILI9341_ROTATION);
}

#if(ILI9341_USE_SD_CARD)
//binary files
uint8_t ILI9341_DrawImageFromSD(uint16_t x, uint16_t y, const TCHAR* path, uint16_t w, uint16_t h)
{
	FRESULT ImageFsResult;
	FIL ImageFile;

	uint8_t Data[SD_BUFFER_SIZE];
	UINT Length;

	ImageFsResult = f_open(&ImageFile, path, FA_READ);

	if(ImageFsResult != FR_OK)
	{
		return 1;
	}
	else
	{
		ILI9341_SetAddrWindow(x, y, w, h);
		ILI9341_SendCommand(ILI9341_RAMWR);

		do
		{
			Length = 0;
			f_read(&ImageFile, Data, SD_BUFFER_SIZE, &Length);
#if (ILI9341_USE_CS == 1)
	ILI9341_CS_LOW;
#endif

			ILI9341_DC_HIGH;
			ILI9341_SendToTFT(Data, Length);

#if (ILI9341_USE_CS == 1)
	ILI9341_CS_HIGH;
#endif
		}while(Length > 0);

		ImageFsResult = f_close(&ImageFile);
		return 0;
	}
}
#endif //ILI9341_USE_SD_CARD

#if(ILI9341_USE_JPEG == 1)

void ConvertRGB888toRGB565(uint8_t *OutputColor, uint32_t sourceColor)
{
	uint32_t red = (sourceColor & 0x00FF0000) >> 16;
	uint32_t green = (sourceColor & 0x0000FF00) >> 8;
	uint32_t blue =  sourceColor & 0x000000FF;
	uint16_t Color16b = ((red >> 3) << 11) + ((green >> 2) << 5) + (blue >> 3);
	OutputColor[1] = Color16b & 0xFF;
	OutputColor[0] = Color16b >> 8;
}


uint8_t ILI9341_DrawJPEG(uint16_t x, uint16_t y, const TCHAR* path)
{
	FIL ImageFile;

	if(f_open(&ImageFile, path, FA_READ) == FR_OK)
	{
		/* Decode JPEG Image */
		JSAMPROW buffer[2] = {0}; /* Output row buffer */
		//uint32_t row_stride = 0; /* Physical row width in image buffer */

		buffer[0] = (JSAMPROW)&_aucLine;

		/* Step 1: Allocate and initialize JPEG decompression object */
		cinfo.err = jpeg_std_error(&jerr);

		/* Step 2: Initialize the JPEG decompression object */
		jpeg_create_decompress(&cinfo);

		jpeg_stdio_src (&cinfo, &ImageFile);

		/* Step 3: read image parameters with jpeg_read_header() */
		jpeg_read_header(&cinfo, TRUE);

		/* Step 4: set parameters for decompression */
		cinfo.dct_method = JDCT_FLOAT;

		/* Step 5: start decompressor */
		jpeg_start_decompress(&cinfo);

		//row_stride = cinfo.output_width * cinfo.output_components;

		ILI9341_SetAddrWindow(x, y, x+cinfo.output_width, y+cinfo.output_height);
		ILI9341_SendCommand(ILI9341_RAMWR);

		uint16_t RowNumber = 0;
		while (cinfo.output_scanline < cinfo.output_height)
		{
			(void) jpeg_read_scanlines(&cinfo, buffer, 1);

			RGB_matrix = (RGB_typedef*)buffer[0];

			uint32_t counter = 0;

			for(counter = 0; counter < cinfo.output_width; counter++)
			{
				ConvertRGB888toRGB565((uint8_t*)buffer[0]+(counter*2),
				(
					RGB_matrix[counter].R << 16 |
					RGB_matrix[counter].G << 8|
					RGB_matrix[counter].B | 0xFF000000)
				);
			}

#if (ILI9341_USE_CS == 1)
			ILI9341_CS_LOW;
#endif

			ILI9341_DC_HIGH;
			ILI9341_SendToTFT((uint8_t*)buffer[0], (cinfo.output_width)*2);

#if (ILI9341_USE_CS == 1)
			ILI9341_CS_HIGH;
#endif
		RowNumber++;
		if(RowNumber == cinfo.output_height)
		{
			break;
		}
		}
		  /* Step 6: Finish decompression */
		  jpeg_finish_decompress(&cinfo);

		  /* Step 7: Release JPEG decompression object */
		  jpeg_destroy_decompress(&cinfo);

		  f_close(&ImageFile);

		  return 0;
	}
	return 1;
}
#endif
