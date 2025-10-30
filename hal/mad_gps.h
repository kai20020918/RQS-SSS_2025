//--------------------------------------------------------------------------------------------
//Copyright 2019 Mathematical Assist Design Laboratories, Inc.
//All Rights Reserved.
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
#ifndef _MAD_GPS_H
#define _MAD_GPS_H


#include "stdint.h"
#include "stdlib.h"
#include "mad_timer.h"
// #include "hal/mad_flash.h"
#include "mad_gpio.h"
// #include "hal/mad_usart.h"
//#include "mad\mad_M24M01.h"
// #include "mad\mad_gpstx.h"

//--------------------------------------------------------------------------------------------
/*//GPS�̎���
//	mad_GPS_OFF();
//	mad_GPS_INIT();
//	mad_GPS_TrueMode();
  	while(1){

  		bool i;
  		i = mad_GPS_GetData(5);

  		mad_GPS_BufClr();
  	}
*/

typedef enum	enm{
	RCVD_NONE,
	RCVD_TIME,
	RCVD_2D,
	RCVD_3D
}GPS_RCVD;

//--------------------------------------------------------------------------------------------
void	mad_GPS_INIT(void);
void	mad_GPS_ON(void);
void	mad_GPS_OFF(void);
void	mad_GPS_BufClr(void);
bool	mad_GPS_GetData(uint16_t count, GPS_RCVD);	//GPRMC�̍s�������M����̐��𐮂���
bool	mad_GPS_GetZDA(void);	//GPZDA�̍s�������M����̐��𐮂���



void	mad_GPS_TrueMode(void);		//GPS��PC/UART���N���X�ڑ�����A������GPS�ݒ�p�̃��[�h�B���O�̐ݒ�͕s�v�B
void	mad_GPS_Evaluate(void);		//NMEA���b�Z�[�W(GGA��RCM�j�̕]����GPS_DATA�^�ւ̃f�[�^�ۑ�
void	mad_GPS_ZDA_Evaluate(void);	//NMEA���b�Z�[�W(ZDA�j�̎�M�ƕ]����GPS_DATA�^�ւ̃f�[�^�ۑ�


void	mad_GPS_TrueMode(void);		//GPS��PC/UART���N���X�ڑ�����A������GPS�ݒ�p�̃��[�h�B���O�̐ݒ�͕s�v�B
void	mad_GPS_Analysis(void);		//NMEA���b�Z�[�W�̕����Ɨv�f���Ƃ̕�����̕ۑ�
void	mad_GPS_Data2Int(void);		//mad_GPS_Analysis()�ŕ�����ɂ����l��INT�ɕϊ�
void	mad_GPS_Data2Bin_GPSTX(void);

char	*mad_GPS_StrToken(char*, char);	//��؂蕶�����A�����镶����ɑΉ�����strtok�B�����system�̂ق��Ɉڍs���ׂ��H


void	mad_GPS_Int2Mem(void);
void	mad_GPS_Mem2Int(void);





//ZOE-M8G��NMEA�́AGNRMC->GNGGA�̏��B
/*
$GNRMC,040227.00,A,3621.28129,N,13922.35286,E,0.053,,041218,,,A*62
$GNGGA,040227.00,3621.28129,N,13922.35286,E,1,05,1.35,76.2,M,38.6,M,,*7A
$GNRMC,040228.00,A,3621.28128,N,13922.35287,E,0.007,,041218,,,A*6C
$GNGGA,040228.00,3621.28128,N,13922.35287,E,1,05,1.35,76.3,M,38.6,M,,*74
$GNRMC,040229.00,A,3621.28126,N,13922.35288,E,0.039,,041218,,,A*61
$GNGGA,040229.00,3621.28126,N,13922.35288,E,1,05,1.35,76.2,M,38.6,M,,*75
*/
//���c�̕W����66m�B


/*
GNZDA�̃T���v��
$GNZDA,,,,,00,00*56
$GNZDA,064349.00,26,03,2019,00,00*79

GNGGA�̃T���v��
$GNGGA,,,,,,0,00,99.99,,,,,,*56
$GNGGA,064350.00,,,,,0,03,2.85,,,,,,*40
$GNGGA,064351.00,3621.31373,N,13922.35626,E,1,03,2.85,-0.5,M,38.6,M,,*62

*/
//--------------------------------------------------------------------------------------------
typedef struct {
	char		TIME[10];	//����		hhmmss.ss
	char		LAT[11];	//�ܓx		ddmm.mmmmm
	bool		NS;			//�k�܁E���	true = �k�܁Afalse=���
	char		LON[12];	//�o�x		dddmm.mmmmm
	bool		EW;			//���o�E���o	true = ���o�Afalse=���o
	char		SAT[5];		//�g�p�q����	dd
	char		DOP[5];		//HDOP�@�������x�ቺ��
	char		ALT[10];						//���x
	//GNRMC
	char		DATE[10];		//���t	ddmmyy�iUTC)
	GPS_RCVD	RCVD;	//TIME, 2D, 3D

	bool		Error;
	char		DAY[3];		//GPZDA�p
	char		MONTH[3];	//GPZDA�p
	char		YEAR[5];	//GPZDA�p
}GPS_DATA;

typedef struct {
	uint32_t	DATE;	//ddmmyy
	uint32_t	TIME;	//hhmmss
	uint32_t	LAT;	// ddmmmmm	���̏����_�ȉ�3���ȍ~�͐؂�̂āB�����_�͍폜
	uint32_t	LON;	//dddmmmmm�@�@���̏����_�ȉ�3���ȍ~�͐؂�̂āB�����_�͍폜
	uint8_t		SAT;	//ss	�q����
	uint8_t		DOP;	//ss	DOP��10�{�̒l
	int32_t		ALT;	//hhhh
	bool		NS;
	bool		EW;
	uint32_t	LAT_DD;
	uint32_t	LAT_MM;
	uint32_t	LON_DD;
	uint32_t	LON_MM;
	uint32_t	HOUR;
	uint32_t	MIN;
	uint32_t	SEC;

	uint32_t	DAY;
	uint32_t	MONTH;


}GPS_INT;


extern uint8_t mad_GPS_DATA_MEM[16];		//�������ۑ��p�̃o�b�t�@





extern GPS_INT		mad_GPS_INT;
extern GPS_DATA	mad_GPS_DATA, mad_GPS_DATA_backup;

extern uint32_t		mad_GPS_TX_DATA[16];



//--------------------------------------------------------------------------------------------

//u-centersetup_v18.10.zip
//[View]->[Messages views]�ŁA���b�Z�[�W�E�C���h�E���J��
//�E�C���h�E�����̃c���[����AUBX-MSG-MEssage�v���_�E�����j���[
/*
static char GPGSA_OFF[16] = {0xB5, 0x62, 0x06, 0x01, 0x06, 0x00, 0xF0, 0x02, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x1F};
static char GPGSV_OFF[16] = {0xB5, 0x62, 0x06, 0x01, 0x06, 0x00, 0xF0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x24};
static char GPGLL_OFF[16] = {0xB5, 0x62, 0x06, 0x01, 0x06, 0x00, 0xF0, 0x01, 0x00, 0x00, 0x00, 0x00, 0xFE, 0x1A};
static char GPRMC_OFF[16] = {0xB5, 0x62, 0x06, 0x01, 0x06, 0x00, 0xF0, 0x04, 0x00, 0x00, 0x00, 0x00, 0x01, 0x29};
static char GPVTG_OFF[16] = {0xB5, 0x62, 0x06, 0x01, 0x06, 0x00, 0xF0, 0x05, 0x00, 0x00, 0x00, 0x00, 0x02, 0x2E};
static char GPZDA_OFF[16] = {0xB5, 0x62, 0x06, 0x01, 0x06, 0x00, 0xF0, 0x08, 0x00, 0x00, 0x00, 0x00, 0x05, 0x3D};
static char GPGGA_OFF[16] = {0xB5, 0x62, 0x06, 0x01, 0x06, 0x00, 0xF0, 0x00, 0x00, 0x01, 0x00, 0x01, 0xFF, 0x19};
*/
/*
Disable_GPGSA:
db	0xB5, 0x62, 0x06, 0x01, 0x06, 0x00, 0xF0, 0x02, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x1F
Disable_GPGSV:
db	0xB5, 0x62, 0x06, 0x01, 0x06, 0x00, 0xF0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x24
Disable_GPGLL:
db	0xB5, 0x62, 0x06, 0x01, 0x06, 0x00, 0xF0, 0x01, 0x00, 0x00, 0x00, 0x00, 0xFE, 0x1A
Disable_GPRMC:
db	0xB5, 0x62, 0x06, 0x01, 0x06, 0x00, 0xF0, 0x04, 0x00, 0x00, 0x00, 0x00, 0x01, 0x29
Disable_GPVTG:
db	0xB5, 0x62, 0x06, 0x01, 0x06, 0x00, 0xF0, 0x05, 0x00, 0x00, 0x00, 0x00, 0x02, 0x2E
Disable_GPZDA:
db	0xB5, 0x62, 0x06, 0x01, 0x06, 0x00, 0xF0, 0x08, 0x00, 0x00, 0x00, 0x00, 0x05, 0x3D
Enable_GPGGA:
db	0xB5, 0x62, 0x06, 0x01, 0x06, 0x00, 0xF0, 0x00, 0x00, 0x01, 0x00, 0x01, 0xFF, 0x19

Baudrate_115k:
db	0xB5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x01, 0x00, 0x00, 0x00, 0xD0
db	0x08, 0x08, 0x00, 0x00, 0xC2, 0x01, 0x00, 0x07, 0x00, 0x03, 0x00
db	0x00, 0x00, 0x00, 0x00, 0xC8, 0xEE
Baudrate_57600:
db	0xB5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x01, 0x00, 0x00, 0x00, 0xD0
db	0x08, 0x08, 0x00, 0x00, 0xE1, 0x00, 0x00, 0x07, 0x00, 0x03, 0x00
db	0x00, 0x00, 0x00, 0x00, 0xe6, 0x39
Baudrate_38400:
db	0xB5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x01, 0x00, 0x00, 0x00, 0xD0
db	0x08, 0x08, 0x00, 0x00, 0x96, 0x00, 0x00, 0x07, 0x00, 0x03, 0x00
db	0x00, 0x00, 0x00, 0x00, 0x9b, 0x00
Baudrate_19200:
db	0xB5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x01, 0x00, 0x00, 0x00, 0xD0
db	0x08, 0x08, 0x00, 0x00, 0x4b, 0x00, 0x00, 0x07, 0x00, 0x03, 0x00
db	0x00, 0x00, 0x00, 0x00, 0x50, 0xc7
Baudrate_9600:
db	0xB5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x01, 0x00, 0x00, 0x00, 0xD0
db	0x08, 0x08, 0x00, 0x80, 0x25, 0x00, 0x00, 0x07, 0x00, 0x03, 0x00
db	0x00, 0x00, 0x00, 0x00, 0xaa, 0x25
*/
//--------------------------------------------------------------------------------------------

#endif