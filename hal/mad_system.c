//--------------------------------------------------------------------------------------------
//Copyright 2019 Mathematical Assist Design Laboratories, Inc.
//All Rights Reserved.
//--------------------------------------------------------------------------------------------
#include "hal/mad_system.h"
#include "pico/stdlib.h"
#include "hal/mad_gpio.h"
#include "time.h"
// #include "mad_GPS.h"
// #include "mad_FLASH.h"
// #include "em_emu.h"

uint32_t SYSTEMCLOCK;
int data1,data2;
time_t	mad_SYSTEM_TIME;
//--------------------------------------------------------------------------------------------
void	mad_SYSTEM_INIT(void)
{
	stdio_init_all(); //pico用
	// CHIP_Init();							// Initialize the chip
	// CMU_ClockEnable(cmuClock_GPIO, true);	//�@TCXO�N���̂��߂�GPIO�����ݒ�
	mad_GPIO_Init();
//	mad_GPIO_Set(mad_GPIO_TCXO_EN);			//TCXO�N��
	// halInit();								// Initialize the system clocks and other HAL components
//	CMU_OscillatorEnable(cmuOsc_HFXO, true, true);//HFXO��Enable�ɂ���B
//	CMU->HFCLKSEL = 0x0002;					//HFXO��I��
	// SYSTEMCLOCK = 40000000;



/*
	//LFXO�R���g���[�����W�X�^��ݒ肷��
	CMU_LFXOInit_TypeDef lfxoInit = CMU_LFXOINIT_DEFAULT;
	CMU_LFXOInit(&lfxoInit);
	INTERNAL_RTCC_Init(CALENDAR_MODE);		//RTC���J�����_�[���[�h�ŏ�����

  //  �I�V���[�^�����ݒ�(����N���b�N��LFRCO�ɕύX���ĕs�v�ȃI�V���[�^��Disable�ɂ���B**
	CMU_OscillatorEnable(cmuOsc_LFXO,     	true, true);	//LFXO�I�V���[�^��Disable�AWait����B
	CMU_OscillatorEnable(cmuOsc_HFXO,     	true, true);	//HCXO�I�V���[�^��Disable�AWait����B
	CMU_OscillatorEnable(cmuOsc_AUXHFRCO, 	false, true);	//AUXFRCO�I�V���[�^��Disable�AWait����B
	CMU_OscillatorEnable(cmuOsc_ULFRCO, 	false, true);	//ULFRCO�I�V���[�^��Disable�AWait����B
	CMU_OscillatorEnable(cmuOsc_LFRCO, 		false, true);	//LFRCO�I�V���[�^��Enable�AWait����B
	CMU_OscillatorEnable(cmuOsc_HFRCO, 		false, true);	//HFRCO�I�V���[�^��Disable�AWait����B

	CMU_ClockSelectSet  (cmuClock_LFA,	cmuSelect_LFXO);	//LFA��LFXO�ɐݒ�
	CMU_ClockSelectSet  (cmuClock_HF,	cmuSelect_HFXO);	//HF��HFXO�ɐݒ�
*/

//	mad_RF_radioInit();		//RF�������ARAIL�̃C���X�^���X���擾

//	mad_FLASH_INIT();
	// mad_FLASH_READ();
//	INTERNAL_RTCC_Init(CALENDAR_MODE);

}
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
// void		mad_SYSTEM_TIME_RTC2SYSTIME(void)
// {
// 	uint32_t	date, time;//, tmp;
// //	char tmpp[8], tmp_date;
// 	struct	tm	tm_time;

// 	date = RTCC_DateGet() & 0xFFFFFF;
// 	time = RTCC_TimeGet();

// 	tm_time.tm_year = ((((date & 0xF00000)>>20) * 10) + ((date & 0x0F0000)>>16)) + 100;	//1900����̔N��
// 	tm_time.tm_mon  = ((((date & 0x00F000)>>12) * 10) + ((date & 0x000F00)>>8))   - 1;	//�O�`11��
// 	tm_time.tm_mday = ((((date & 0x0000F0)>>4 ) * 10) + ((date & 0x00000F)   ));		//�P�`31��
// 	tm_time.tm_hour = ((((time & 0xF00000)>>20) * 10) + ((time & 0x0F0000)>>16));	//0�`23��
// 	tm_time.tm_min  = ((((time & 0x00F000)>>12) * 10) + ((time & 0x000F00)>>8)) ;	//0�`23��
// 	tm_time.tm_sec  = ((((time & 0x0000F0)>>4 ) * 10) + ((time & 0x00000F)   )) ;	//0�`31��

// 	mad_SYSTEM_TIME = mktime(&tm_time);
// 	sprintf(mad_UART1_RX_BUF.data,"%s", ctime(&mad_SYSTEM_TIME));
// }
// //--------------------------------------------------------------------------------------------
// void	mad_SYSTEM_TIME_GPS2SYSTIME(void)
// {
// 	struct	tm	tm_time;
// 	char tmp[3];
// 	tmp[2] = 0x00;
// 	tmp[0] = mad_GPS_DATA.DATE[4];	tmp[1] = mad_GPS_DATA.DATE[5];
// 	tm_time.tm_year = (atoi(tmp) + 100);
// 	tmp[0] = mad_GPS_DATA.DATE[2];	tmp[1] = mad_GPS_DATA.DATE[3];
// 	tm_time.tm_mon = atoi(tmp) - 1;	//1����0
// 	tmp[0] = mad_GPS_DATA.DATE[0];	tmp[1] = mad_GPS_DATA.DATE[1];
// 	tm_time.tm_mday = atoi(tmp) ;	//1����1

// 	tmp[0] = mad_GPS_DATA.TIME[0];	tmp[1] = mad_GPS_DATA.TIME[1];
// //	tm_time.tm_hour = atoi(tmp) + mad_FLASH[mad_FLASH_LOCALTIME];		//�����Ń��[�J���^�C���ɕϊ����Ă���B
// 	tmp[0] = mad_GPS_DATA.TIME[2];	tmp[1] = mad_GPS_DATA.TIME[3];
// 	tm_time.tm_min = atoi(tmp);
// 	tmp[0] = mad_GPS_DATA.TIME[4];	tmp[1] = mad_GPS_DATA.TIME[5];
// 	tm_time.tm_sec = atoi(tmp);
// 	tm_time.tm_isdst = -1;

// 	mad_SYSTEM_TIME = mktime(&tm_time);
// //	sprintf(mad_UART1_RX_BUF.data,"%s", ctime(&mad_SYSTEM_TIME));
// }
// //--------------------------------------------------------------------------------------------
// void	mad_SYSTEM_TIME_ZDA2SYSTIME(void)
// {
// 	struct	tm	tm_time;
// 	char tmp[3];
// 	tm_time.tm_year = atoi(mad_GPS_DATA.YEAR) - 1900;
// 	tm_time.tm_mon  = atoi(mad_GPS_DATA.MONTH) - 1;	//1����0
// 	tm_time.tm_mday = atoi(mad_GPS_DATA.DAY) ;	//1����1

// 	tmp[0] = mad_GPS_DATA.TIME[0];	tmp[1] = mad_GPS_DATA.TIME[1];
// 	tm_time.tm_hour = atoi(tmp);
// 	tmp[0] = mad_GPS_DATA.TIME[2];	tmp[1] = mad_GPS_DATA.TIME[3];
// 	tm_time.tm_min = atoi(tmp);
// 	tmp[0] = mad_GPS_DATA.TIME[4];	tmp[1] = mad_GPS_DATA.TIME[5];
// 	tm_time.tm_sec = atoi(tmp);
// 	tm_time.tm_isdst = -1;			//���̒l�Ȃ�ΉĎ��Ԃ͗L���ɂȂ�A0 �Ȃ�Ζ����A���̒l�Ȃ�΂��̏��ɂ� �Ӗ����Ȃ��B

// 	mad_SYSTEM_TIME = mktime(&tm_time);

// //	char tmpc[255];
// //	sprintf(tmpc,"%s", ctime(&mad_SYSTEM_TIME));
// }
// //--------------------------------------------------------------------------------------------
// void	mad_SYSTEM_TIME_SYSTIME2GPSDATA(void)
// {
// 	struct	tm*	tm_time;

// 	tm_time = localtime(&mad_SYSTEM_TIME);

// //	tm_time->tm_hour -= mad_FLASH[mad_FLASH_LOCALTIME];		//���[�J��������������
// 	mad_SYSTEM_TIME = mktime(tm_time);						//�����ǃV�X�e���^�C���ϐ��ɏ����߂���

// 	tm_time = localtime(&mad_SYSTEM_TIME);			  		//tm�^�ɂ���


// 	sprintf(mad_GPS_DATA.DATE, "%02d%02d%02d",
// 			tm_time->tm_mday,
// 			tm_time->tm_mon +1,
// 			tm_time->tm_year - 100);

// 	sprintf(mad_GPS_DATA.TIME, "%02d%02d%02d",
// 			tm_time->tm_hour,
// 			tm_time->tm_min,
// 			tm_time->tm_sec);
// }
// //--------------------------------------------------------------------------------------------
// uint32_t	mad_SYSTEM_TIME_SYSTIME2RTC_DATE(void)
// {
// //tm�`����RTC�����ɕϊ�
// 	uint32_t	bcd;
// 	struct	tm*	tm_time;
// 	tm_time = localtime(&mad_SYSTEM_TIME);
// 	bcd  = ( (((tm_time->tm_year-100)/100)<<8)|((((tm_time->tm_year-100) %100)/10)<<4)|((tm_time->tm_year-100) %10)) << 16;
// 	bcd += ( (((tm_time->tm_mon+1) /100)<<8)|((((tm_time->tm_mon+1)  %100)/10)<<4)|((tm_time->tm_mon+1)  %10)) << 8;
// 	bcd += ( ((tm_time->tm_mday /100)<<8)|(((tm_time->tm_mday  %100)/10)<<4)|(tm_time->tm_mday  %10));

// 	return bcd;
// }
// //--------------------------------------------------------------------------------------------
// uint32_t	mad_SYSTEM_TIME_SYSTIME2RTC_TIME(void)
// {
// //tm�`����RTC�����b�ɕϊ�
// 	uint32_t	bcd;
// 	struct	tm*	tm_time;
// 	tm_time = localtime(&mad_SYSTEM_TIME);
// 	bcd  = ( ((tm_time->tm_hour/100)<<8)|(((tm_time->tm_hour %100)/10)<<4)|(tm_time->tm_hour %10)) << 16;
// 	bcd += ( ((tm_time->tm_min /100)<<8)|(((tm_time->tm_min  %100)/10)<<4)|(tm_time->tm_min  %10)) << 8;
// 	bcd += ( ((tm_time->tm_sec /100)<<8)|(((tm_time->tm_sec  %100)/10)<<4)|(tm_time->tm_sec  %10));

// 	return bcd;
// }
// //--------------------------------------------------------------------------------------------
// //--------------------------------------------------------------------------------------------
// //--------------------------------------------------------------------------------------------
// //--------------------------------------------------------------------------------------------
// void	mad_SYSTEM_EM3(void)
// {
// 	mad_USART1_RxStop();		//UART��M���~
// 	mad_GPS_OFF();				//GPS�d��OFF
// 	mad_RF_TX_STOP();
// //	mad_DAC_DISABLE();

// //	mad_SYSTEM_HFRCO();			//LFXO�N���b�N�Ɉڍs
// 	mad_SYSTEM_LFXO();			//LFXO�N���b�N�Ɉڍs
// //	CMU->HFCLKSEL = 0x0004;//LFXO��I��


// //	CMU_OscillatorEnable(cmuOsc_HFXO,  false, false);//HFXO��Disable�ɂ���B

// 	mad_GPIO_Sleep();
// 	EMU_EnterEM3(true);
// }
// //--------------------------------------------------------------------------------------------
// void	mad_SYSTEM_LFXO(void)
// {
// 	mad_SYSTEM_HFXO();
// 	//LFXO�œ���
// 	mad_DAC_DISABLE();

// //	CMU_LFXOInit_TypeDef lfxoInit = CMU_LFXOINIT_DEFAULT;
// //	CMU_LFXOInit(&lfxoInit);

// 	CMU_OscillatorEnable(cmuOsc_LFXO, true, true);//LFXO�I�V���[�^��Enable�AWait����B
// //	CMU->HFCLKSEL = 0x0004;//LFXO��I��
// 	CMU_ClockSelectSet  (cmuClock_HF,	cmuSelect_LFXO);	//HF��HFRCO�ɐݒ�
// 	CMU_OscillatorEnable(cmuOsc_HFXO, false, false);//HFXO��Disable�ɂ���B
// //	mad_GPIO_Clr(mad_GPIO_TCXO_EN);	//TCXO OFF


// //	HFRCO		0x01
// //	HFXO		0x02
// //	LFRCO		0x03
// //	LFXO		0x04
// //	HFRCODIV2	0x05
// //	CLKIN		0x07
// }
// //--------------------------------------------------------------------------------------------
// void	mad_SYSTEM_HFXO(void)
// {
// //	mad_GPIO_Set(mad_GPIO_TCXO_EN);//TCXO�I�V���[�^�N��
// 	CMU_OscillatorEnable(cmuOsc_HFXO, true, true);//HFXO��Enable�ɂ���B
// 	CMU->HFCLKSEL = 0x0002;//HFXO��I��
// 	mad_GPIO_WakeUp();
// //		CMU_OscillatorEnable(cmuOsc_LFXO, false, true);//LFXO��Disable�ɂ���ƁARTC���~�܂��Ă��܂��̂ŁA�~�߂Ă͂����Ȃ��B
// 	SYSTEMCLOCK = 40000000;
// }
// //--------------------------------------------------------------------------------------------
// void	mad_SYSTEM_LFRCO(void){}
// //--------------------------------------------------------------------------------------------
// void	mad_SYSTEM_HFRCO(void)
// {
// 	CMU_OscillatorEnable(cmuOsc_HFRCO, true, true);	//HFRCO��Enable�ɂ���B
// 	CMU_HFRCOBandSet(cmuHFRCOFreq_7M0Hz);			//���g���̃Z�b�g
// 	CMU_ClockSelectSet  (cmuClock_HF,	cmuSelect_HFRCO);	//HF��HFRCO�ɐݒ�
// 	CMU->HFCLKSEL = 0x0001;//HFRCO��I��

// //	mad_GPIO_Clr(mad_GPIO_TCXO_EN);	//TCXO OFF
// 	SYSTEMCLOCK = 7000000;
// //	mad_GPIO_Clr(mad_GPIO_TCXO_EN);	//TCXO OFF
// /*
//  cmuHFRCOFreq_1M0Hz  	1MHz RC band
//  cmuHFRCOFreq_2M0Hz  	2MHz RC band
//  cmuHFRCOFreq_4M0Hz  	4MHz RC band
//  cmuHFRCOFreq_7M0Hz  	7MHz RC band
//  cmuHFRCOFreq_13M0Hz  	13MHz RC band
//  cmuHFRCOFreq_16M0Hz  	16MHz RC band
//  cmuHFRCOFreq_19M0Hz  	19MHz RC band
//  cmuHFRCOFreq_26M0Hz	26MHz RC band
// cmuHFRCOFreq_32M0Hz 	32MHz RC band
//  cmuHFRCOFreq_38M0Hz	38MHz RC band
//  */
// }
// //--------------------------------------------------------------------------------------------
// /*
// //�N���b�N�\�[�X��m����@
//  CMU_Select_TypeDef	x;
// x = CMU_ClockSelectGet(cmuClock_HF);		//�����́A���L�S�̂����ꂩ�B�@�@cmuClock_HF cmuClock_LFA cmuClock_LFB cmuClock_DBG		//�I������Ă���N���b�N��m��
// x = CMU_ClockSelectGet(cmuClock_LFA);		//�����́A���L�S�̂����ꂩ�B�@�@cmuClock_HF cmuClock_LFA cmuClock_LFB cmuClock_DBG
// */
// //--------------------------------------------------------------------------------------------
