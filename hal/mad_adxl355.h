//--------------------------------------------------------------------------------------------
//Copyright 2019 Mathematical Assist Design Laboratories, Inc.
//All Rights Reserved.
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
#ifndef _MAD_ADXL355_H
#define _MAD_ADXL355_H
//--------------------------------------------------------------------------------------------
/*

#include "stdlib.h"
*/

#include "pico/stdlib.h"
// #include "stdbool.h"
// #include "stdint.h"
#include "mad_gpio.h"
#include "mad_timer.h"

//--------------------------------------------------------------------------------------------

#define	ADXL355_Read_cmd 	0x3B	//(0x1d << 1) + 1		//0x1d = x001 1101, cmd = 0011101x	0x3A,	0x3B
#define	ADXL355_Write_cmd 	0x3A	//0x1d << 1

//#define	ADXL355_Read_cmd 	0xA7	//(0x1d << 1) + 1		//0x1d = x001 1101, cmd = 0011101x	0x3A,	0x3B
//#define	ADXL355_Write_cmd 	0xA6	//0x1d << 1


//#define		SPS500			//500SPS
#define		SPS100				//100SPS


#define	ADXL355_CH	4
#define	ADXL355_AXIS 3

#define ODR_4000Hz	0x00
#define ODR_2000Hz	0x01
#define ODR_1000Hz	0x02
#define	ODR_500HZ	0x03
#define	ODR_250HZ	0x04
#define	ODR_125HZ	0x05
#define	ODR_62R5HZ	0x06
#define	ODR_31R25HZ	0x07


#define	AXIS_X	0
#define	AXIS_Y	1
#define	AXIS_Z	2

#define	CH1	0
#define	CH2	1
#define	CH3	2
#define	CH4 3


extern uint32_t	ADXL355_BYTE_DATA[ADXL355_CH];
extern int32_t	ADXL_AXEL_DATA[ADXL355_CH][ADXL355_AXIS];
extern unsigned char ADXL_AXEL_TX_DATA[ADXL355_CH][ADXL355_AXIS];


//uint32_t	ADXL355_BYTE_DATA[ADXL355_CH];
//int32_t	ADXL_AXEL_DATA[CH][AXIS];


#define	ADXL355_AXEL_DATA_START_ADR	0x08

#define ADXL355_CH1_X_H		8		//X-axis data[19:12]
#define ADXL355_CH1_X_M		9		//X-axis data[11:4]
#define ADXL355_CH1_X_L		10		//X-axis data[3:0]
#define ADXL355_CH1_Y_H		11		//Y-axis data[19:12]
#define ADXL355_CH1_Y_M		12		//Y-axis data[11:4]
#define ADXL355_CH1_Y_L		13		//Y-axis data[3:0]
#define ADXL355_CH1_Z_H		14		//Z-axis data[19:12]
#define ADXL355_CH1_Z_M		15		//Z-axis data[11:4]
#define ADXL355_CH1_Z_L		16		//Z-axis data[3:0]

#define ADXL355_CH2_X_H		17		//X-axis data[19:12]
#define ADXL355_CH2_X_M		18		//X-axis data[11:4]
#define ADXL355_CH2_X_L		19		//X-axis data[3:0]
#define ADXL355_CH2_Y_H		20		//Y-axis data[19:12]
#define ADXL355_CH2_Y_M		21		//Y-axis data[11:4]
#define ADXL355_CH2_Y_L		22		//Y-axis data[3:0]
#define ADXL355_CH2_Z_H		23		//Z-axis data[19:12]
#define ADXL355_CH2_Z_M		24		//Z-axis data[11:4]
#define ADXL355_CH2_Z_L		25		//Z-axis data[3:0]

#define ADXL355_CH3_X_H		26		//X-axis data[19:12]
#define ADXL355_CH3_X_M		27		//X-axis data[11:4]
#define ADXL355_CH3_X_L		28		//X-axis data[3:0]
#define ADXL355_CH3_Y_H		29		//Y-axis data[19:12]
#define ADXL355_CH3_Y_M		30		//Y-axis data[11:4]
#define ADXL355_CH3_Y_L		31		//Y-axis data[3:0]
#define ADXL355_CH3_Z_H		32		//Z-axis data[19:12]
#define ADXL355_CH3_Z_M		33		//Z-axis data[11:4]
#define ADXL355_CH3_Z_L		34		//Z-axis data[3:0]

#define ADXL355_CH4_X_H		35		//X-axis data[19:12]
#define ADXL355_CH4_X_M		36		//X-axis data[11:4]
#define ADXL355_CH4_X_L		37		//X-axis data[3:0]
#define ADXL355_CH4_Y_H		38		//Y-axis data[19:12]
#define ADXL355_CH4_Y_M		39		//Y-axis data[11:4]
#define ADXL355_CH4_Y_L		40		//Y-axis data[3:0]
#define ADXL355_CH4_Z_H		41		//Z-axis data[19:12]
#define ADXL355_CH4_Z_M		42		//Z-axis data[11:4]
#define ADXL355_CH4_Z_L		43		//Z-axis data[3:0]



void	mad_ADXL355_SCL_Set(void);
void	mad_ADXL355_SCL_Clr(void);
void	mad_ADXL355_SDA_Set(void);
void	mad_ADXL355_SDA_Clr(void);

void	mad_ADXL355_Wait(void);

void	mad_ADXL355_SDA_DIR_Read(void);
void	mad_ADXL355_SDA_DIR_Write(void);
void	mad_ADXL355_Start(void);
void	mad_ADXL355_Stop(void);
void	mad_ADXL355_ACK_Out(void);
void	mad_ADXL355_NAK_Out(void);
void	mad_ADXL355_4ch_Read(void);
uint8_t mad_ADXL355_Read(void);
bool	mad_ADXL355_Write(uint8_t DATA);
void	mad_ADXL355_Init(void);

bool	mad_ADXL355_GetData(void);



bool	mad_ADXL355_BYTE_WRITE(uint32_t register_address, uint8_t data);
bool	mad_ADXL355_BYTE_READ(uint32_t register_address, uint8_t *data);

extern bool	mad_ADXL_355_ERROR_CH1;
extern bool	mad_ADXL_355_ERROR_CH2;
extern bool	mad_ADXL_355_ERROR_CH3;
extern bool	mad_ADXL_355_ERROR_CH4;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif
