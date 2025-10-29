//--------------------------------------------------------------------------------------------
//Copyright 2019 Mathematical Assist Design Laboratories, Inc.
//All Rights Reserved.
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
#ifndef _MAD_TIMER_H
#define _MAD_TIMER_H

//--------------------------------------------------------------------------------------------
#include "stdbool.h"
#include "stdint.h"
// #include "mad_system.h"
//--------------------------------------------------------------------------------------------

//INIT�̈����ł���CountValue�ŋK�肷��Timer�y���t�F�����̃J�E���^�l�ƁA�|�X�g�X�P�[���ł���PostScalerValue�̒l������B
//mad_TIMER1_OverFlow�́A�����݉񐔂�CountValue�@�~�@PostScalerValue�@�ƂȂ����Ƃ��ɁA�@True�ɂȂ�B

void	mad_TIMER1_INIT(uint16_t CountValue, uint32_t PostScaleValue);		//�����́A�I�[�g�����[�h�J�E���^�̍ő�l�@//	725.625usec / ( 1 / 40MHz) = 29025clock;
void	mad_TIMER1_ResetTimerCounter(void);	//�����́A�I�[�g�����[�h�J�E���^�̒l
void	mad_TIMER1_ENABLE(void);
void	mad_TIMER1_DISABLE(void);
void	mad_TIMER1_WAIT_10ms(uint16_t);
void    mad_WAIT_us(uint32_t us);

extern uint32_t	mad_TIMER1_CountValue;
extern uint32_t	mad_TIMER1_PostScale;
extern uint32_t	mad_TIMER1_PostScale_Default;
extern bool		mad_TIMER1_OverFlow;

//bool	s;

#endif
