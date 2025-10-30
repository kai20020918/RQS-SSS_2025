//--------------------------------------------------------------------------------------------
//Copyright 2019 Mathematical Assist Design Laboratories, Inc.
//All Rights Reserved.
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
#ifndef _MAD_USART_H
#define _MAD_USART_H

#include <string.h>
#include <stdio.h>
#include "pico/stdlib.h"
//--------------------------------------------------------------------------------------------
/* Declare a circular buffer structure to use for Rx and Tx queues */
// mad_usart.h に含まれるべき定義（仮定）

#define UART1_BUFFERSIZE 512 // GPS用
#define UART0_BUFFERSIZE 512  // PC用


typedef struct {
    char data[UART1_BUFFERSIZE]; 
    volatile uint32_t index;     // ★ volatile 追加 (割り込みで変更されるため)
    volatile bool empty;         // ★ volatile 追加
    volatile bool overflow;      // ★ volatile 追加
    volatile uint8_t enter;      // ★ volatile 追加
} mad_UART1_RX_BUF_t; // UART1 専用の型

typedef struct {
    char data[UART0_BUFFERSIZE];
    volatile uint32_t index;
    volatile bool empty;
    volatile bool overflow;
    volatile uint8_t enter;
} mad_UART0_RX_BUF_t; // UART0 専用の型


extern mad_UART0_RX_BUF_t mad_UART0_RX_BUF; // PC用
extern mad_UART1_RX_BUF_t mad_UART1_RX_BUF; // GPS用

//--------------------------------------------------------------------------------------------
void	mad_USART0_INIT(uint64_t baudrate);
void	mad_USART1_INIT(uint64_t baudrate);

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

#endif
