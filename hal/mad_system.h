//--------------------------------------------------------------------------------------------
//Copyright 2019 Mathematical Assist Design Laboratories, Inc.
//All Rights Reserved.
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
#ifndef _MAD_SYSTEM_H
#define _MAD_SYSTEM_H
//--------------------------------------------------------------------------------------------
// #include "mad_GPIO.h"
// #include "em_cmu.h"
// #include "em_chip.h"
#include "time.h"
// #include "em_rtcc.h"
//--------------------------------------------------------------------------------------------
#define COUNTER_MODE 0
#define CALENDAR_MODE 1


uint32_t SYSTEMCLOCK;		//CPU�N���b�N�̎��g���AHz�P��
//--------------------------------------------------------------------------------------------
void	mad_SYSTEM_INIT(void);

void	mad_SYSTEM_LFXO(void);
void	mad_SYSTEM_HFXO(void);
void	mad_SYSTEM_LFRCO(void);
void	mad_SYSTEM_HFRCO(void);
void	mad_SYSTEM_EM3(void);
/*
CMU_OscillatorEnable(cmuOsc_LFXO,     	true, true);//HFRCO�I�V���[�^��Disable�AWait����B
CMU_OscillatorEnable(cmuOsc_HFXO,     	true, true);//HFRCO�I�V���[�^��Disable�AWait����B
CMU_OscillatorEnable(cmuOsc_AUXHFRCO, 	false, true);//HFRCO�I�V���[�^��Disable�AWait����B
CMU_OscillatorEnable(cmuOsc_ULFRCO, 	false, true);//HFRCO�I�V���[�^��Disable�AWait����B
CMU_OscillatorEnable(cmuOsc_LFRCO, 		false, true);//LFRCO�I�V���[�^��Enable�AWait����B
CMU_OscillatorEnable(cmuOsc_HFRCO, 		false, true);//HFRCO�I�V���[�^��Disable�AWait����B
*/

//GPS�����ARTC�����Atime.h�����̂R������B
//GPS������ǂ݁Atime.h�`���ɕϊ�����B
//time.h�`���ŁA�����̌v�Z������B
//time.h�`����RTC�����`���ɕϊ�����B		���ݎ�����A���[������������������B

void		mad_SYSTEM_TIME_GPS2SYSTIME(void);		//GPS�̓������V�X�e�������ɐݒ肷��B���̎��_�Ŏ����̕��𑫂���������B���{��+9�B
void		mad_SYSTEM_TIME_ZDA2SYSTIME(void);		//GPS��ZDA�̓������V�X�e�������ɐݒ肷��B
void		mad_SYSTEM_TIME_SYSTIME2GPSDATA(void);	//�V�X�e��������GPSDATA�\���̂ɕϊ����� �������ւ̕ۑ��p�H
uint32_t	mad_SYSTEM_TIME_SYSTIME2RTC_DATE(void);	//�V�X�e��������RTC�N�����ɕϊ�
uint32_t	mad_SYSTEM_TIME_SYSTIME2RTC_TIME(void);	//�V�X�e��������RTC�����b�ɕϊ�
void		mad_SYSTEM_TIME_RTC2SYSTIME(void);

int data1,data2;

void		mad_SYSTEM_TIME_ClockSet(void);		//tm�`���Ō��ݎ�����ݒ肷��
void		mad_SYSTEM_TIME_AlarmSet(void);		//tm�`���ŃA���[��������ݒ肷��

	time_t	mad_SYSTEM_TIME;


//--------------------------------------------------------------------------------------------

#endif
