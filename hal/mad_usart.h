//--------------------------------------------------------------------------------------------
//Copyright 2019 Mathematical Assist Design Laboratories, Inc.
//All Rights Reserved.
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
#ifndef _MAD_USART_H
#define _MAD_USART_H

#include <string.h>
#include <stdio.h>
//--------------------------------------------------------------------------------------------
/* Declare a circular buffer structure to use for Rx and Tx queues */
// mad_usart.h に含まれるべき定義（仮定）

#define BUFFERSIZE 512 // GPSバッファサイズ
typedef struct {
	char data[512]; // UART1 (GPS) 用
	uint32_t index;
	bool empty;
	bool overflow;
	uint8_t enter;
} mad_UART_RX_BUF;

extern mad_UART_RX_BUF mad_UART0_RX_BUF; // PC用 (data[16]などサイズは要調整)
extern mad_UART_RX_BUF mad_UART1_RX_BUF; // GPS用

//--------------------------------------------------------------------------------------------
void	mad_USART0_INIT(uint64_t baudrate);
void	mad_USART0_TxStr(char *data);
void	mad_USART1_TxStr(char *data);
void	mad_USART0_TxChar(char data);
void	mad_USART1_TxChar(char data);
void	mad_USART0_TxStrCksum(char *data);
void	mad_USART0_RxBufClr(void);
void	mad_USART1_RxBufClr(void);
bool	mad_USART0_RxCkSumCheck(char *data);
bool	mad_USART0_RxCkSumCheck4char(void);

void	mad_USART1_RxStart(void);
void	mad_USART1_RxStop(void);

void	mad_USART0_RxStart(void);
void	mad_USART0_RxStop(void);

void	mad_USART0_TxPrompt(void);
void	mad_USART0_TxError(void);

void	mad_USART1_INIT(uint64_t baudrate);
#endif
