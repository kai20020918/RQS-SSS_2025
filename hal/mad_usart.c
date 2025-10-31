// mad_usart.c (RP2350 移植版)
//--------------------------------------------------------------------------------------------
//Copyright 2019 Mathematical Assist Design Laboratories, Inc.
//All Rights Reserved.
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
#include <string.h>
#include <stdlib.h>
#include <stdio.h> // sprintf のため

#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/irq.h"
#include "hardware/gpio.h"
#include "pico/sync.h"

#include "mad_usart.h" 
#include "mad_gpio.h"  
#include "rp2350_pin_config.h" 

// 割り込みハンドラのプロトタイプ
static void mad_uart0_rx_irq_handler(void);
static void mad_uart1_rx_irq_handler(void);

// 補助関数: 受信バッファの状態を取得
// 注: mad_usart.h で mad_UARTx_RX_BUF の実体 (mad_usart.c内) と extern 宣言 (mad_usart.h内)が必要です。
extern mad_UART0_RX_BUF_t mad_UART0_RX_BUF;
extern mad_UART1_RX_BUF_t mad_UART1_RX_BUF;

volatile int g_uart1_irq_count = 0; // UART1 IRQ デバッグ用カウンターの実体をここで定義

mad_UART0_RX_BUF_t mad_UART0_RX_BUF = {
    .data = {0},
    .index = 0,
    .empty = true,
    .overflow = false,
    .enter = 0
};

// mad_UART1_RX_BUF のサイズは mad_usart.h の定義（512）を使用
mad_UART1_RX_BUF_t mad_UART1_RX_BUF = {
    .data = {0},
    .index = 0,
    .empty = true,
    .overflow = false,
    .enter = 0
};
//--------------------------------------------------------------------------------------------



void mad_USART0_RxBufClr(void){
    uint32_t saved_irq = save_and_disable_interrupts(); //割り込みを一時無効化
    mad_UART0_RX_BUF.index = 0;
    mad_UART0_RX_BUF.empty = true;
    mad_UART0_RX_BUF.overflow = false;
    mad_UART0_RX_BUF.enter = 0;
    // data配列のサイズは mad_UART0_RX_BUF の定義による
    memset(mad_UART0_RX_BUF.data, 0x00, sizeof(mad_UART0_RX_BUF.data)); 
    restore_interrupts(saved_irq); // ★★★ この行を追加 ★★★
}
//--------------------------------------------------------------------------------------------
void mad_USART1_RxBufClr(void){   
    uint32_t saved_irq = save_and_disable_interrupts();
    mad_UART1_RX_BUF.index = 0;
    mad_UART1_RX_BUF.empty = true;
    mad_UART1_RX_BUF.overflow = false;
    mad_UART1_RX_BUF.enter = 0;
    memset(mad_UART1_RX_BUF.data, 0x00, sizeof(mad_UART1_RX_BUF.data));
    restore_interrupts(saved_irq); // ★★★ この行を追加 ★★★
}
//--------------------------------------------------------------------------------------------
void mad_USART1_INIT(uint64_t baudrate){
    printf("mad_USART1_INIT: Starting (for GPS - uart1) with requested baudrate %lu...\n", (uint32_t)baudrate);
    fflush(stdout);

    // ★★★ 修正: PICO_GPS_UART_INSTANCE (uart1) を使う ★★★
    uint actual_baudrate = uart_init(PICO_GPS_UART_INSTANCE, (uint32_t)baudrate);
    printf("mad_USART1_INIT: uart_init done. Actual baudrate: %u\n", actual_baudrate);
    fflush(stdout);

    if (actual_baudrate == 0) {
        printf("mad_USART1_INIT: Failed to set baudrate!\n"); fflush(stdout);
        return; // 初期化失敗
    }

    // --- ピン機能設定 (GPSピン) ---
    gpio_set_function(PIN_GPS_UART_TX, GPIO_FUNC_UART);
    printf("mad_USART1_INIT: gpio_set_function TX done.\n"); fflush(stdout);
    gpio_set_function(PIN_GPS_UART_RX, GPIO_FUNC_UART);
    printf("mad_USART1_INIT: gpio_set_function RX done.\n"); fflush(stdout);

    // --- ★★★ 修正: stdio_uart_init_full は UART0 用なのでここでは呼ばない ★★★ ---
    
    // --- IRQ 関連 (UART1) ---
    int uart_irq = (PICO_GPS_UART_INSTANCE == uart0) ? UART0_IRQ : UART1_IRQ; // UART1_IRQ
    irq_set_exclusive_handler(uart_irq, mad_uart1_rx_irq_handler);
    printf("mad_USART1_INIT: irq_set_exclusive_handler done.\n"); fflush(stdout);
    irq_set_enabled(uart_irq, true);
    printf("mad_USART1_INIT: irq_set_enabled done.\n"); fflush(stdout);
    
    uart_set_irq_enables(PICO_GPS_UART_INSTANCE, false, false); // 受信は RxStart で開始
    printf("mad_USART1_INIT: uart_set_irq_enables done.\n"); fflush(stdout);

    mad_USART1_RxBufClr();
    printf("mad_USART1_INIT: RxBufClr done. Finished.\n"); fflush(stdout);
}//--------------------------------------------------------------------------------------------
void mad_USART0_RxStart(void)
{
    // 受信割り込みを有効化 (Tx割り込みは使用しない)
    // Pico SDKのUART割り込みは、TX/RXで1つ
    uart_set_irq_enables(PICO_PC_UART_INSTANCE, true, false); 
}
//--------------------------------------------------------------------------------------------
void mad_USART0_RxStop(void)
{
    // 受信割り込みを無効化
    uart_set_irq_enables(PICO_PC_UART_INSTANCE, false, false);
}
//--------------------------------------------------------------------------------------------
void mad_USART1_RxStart(void)
{
    // 受信割り込みを有効化
    uart_set_irq_enables(PICO_GPS_UART_INSTANCE, true, false); 
}
//--------------------------------------------------------------------------------------------
void mad_USART1_RxStop(void)
{
    // 受信割り込みを無効化
    uart_set_irq_enables(PICO_GPS_UART_INSTANCE, false, false);
}
//--------------------------------------------------------------------------------------------
void mad_USART0_INIT(uint64_t baudrate)
{
    printf("mad_USART0_INIT: Setting up RX Interrupt for UART0 (stdio)...\n");
    fflush(stdout);
    
    // 1. UARTペリフェラルとピンの初期化は mad_SYSTEM_INIT が stdio 用に実行済み
    //    ここでは何もしない (重複呼び出しを避ける)
    
    // 2. 割り込みハンドラの登録と有効化
    irq_set_exclusive_handler(UART0_IRQ, mad_uart0_rx_irq_handler);
    irq_set_enabled(UART0_IRQ, true);
    printf("mad_USART0_INIT: IRQ handler set.\n");
    fflush(stdout);

    // 3. 受信バッファのクリアと受信開始
    mad_USART0_RxBufClr();
    mad_USART0_RxStart(); // PC からのコマンド受信をすぐに開始
    printf("mad_USART0_INIT: RX Buffer cleared and RX started.\n");
    fflush(stdout);
}
//--------------------------------------------------------------------------------------------
// (mad_USART0_RxCkSumCheck, mad_USART0_RxCkSumCheck4char, mad_USART0_TxErrorはそのまま)
// (ここでは省略しますが、元のコードをそのまま貼り付けて問題ありません)
bool	mad_USART0_RxCkSumCheck(char *rxdata){
	  char *tp;
	  strcpy(rxdata,mad_UART0_RX_BUF.data);

	  tp = strtok(rxdata, "*");
	  tp = strtok(NULL, "*");

	  uint8_t cksum = strtol(tp, (char **) NULL, 16);

	  strcpy(rxdata,mad_UART0_RX_BUF.data);
	  int count = strlen(rxdata);
	  uint8_t tmp = 0;

	  for(int i = 0; i< count ; i++){
		  tmp += rxdata[i];
	  }
	  if(tmp == cksum)return true;
	  else	return false;
}

bool	mad_USART0_RxCkSumCheck4char(void){
	if(mad_UART0_RX_BUF.data[0] != 'C')		return false;
	uint8_t	cksum;
	cksum =  mad_UART0_RX_BUF.data[0];
	cksum += mad_UART0_RX_BUF.data[1];
	cksum += mad_UART0_RX_BUF.data[2];
	if(cksum == mad_UART0_RX_BUF.data[3])		return true;
	return false;
}

void	mad_USART0_TxError(void){
//	mad_USART0_TxStr("\nERROR>");
//	mad_USART0_RxBufClr();
}
//--------------------------------------------------------------------------------------------
void mad_USART0_TxPrompt(void)
{
    uart_putc_raw(PICO_PC_UART_INSTANCE, '\n');
    uart_putc_raw(PICO_PC_UART_INSTANCE, '>');
    mad_USART0_RxBufClr();
}
//--------------------------------------------------------------------------------------------
void mad_USART0_TxStr(char *data)
{
    uart_puts(PICO_PC_UART_INSTANCE, data);
}
//--------------------------------------------------------------------------------------------
void mad_USART1_TxStr(char *data)
{
    uart_puts(PICO_GPS_UART_INSTANCE, data);
    // 元のコードでは TxStr 後にバッファクリアしているが、GPS通信の一般的な仕様に反するためコメントアウト推奨
    // mad_USART1_RxBufClr(); 
}
//--------------------------------------------------------------------------------------------
void mad_USART0_TxChar(char data)
{
    //　PCへの送信
    uart_putc_raw(PICO_PC_UART_INSTANCE, data);
}
//--------------------------------------------------------------------------------------------
void mad_USART1_TxChar(char data)
{
    // GPSへの送信
    uart_putc_raw(PICO_GPS_UART_INSTANCE, data);
}
//--------------------------------------------------------------------------------------------
void	mad_USART0_TxStrCksum(char *data)
{
	  size_t	count;
	  uint8_t	cksum = 0;
	  char cksumHex[4];

	  count = strlen(data);
	  for(uint8_t i = 0; i < count; i++){
		  cksum += data[i];
          // ★ 修正: USART_Tx -> uart_putc_raw ★
		  uart_putc_raw(PICO_PC_UART_INSTANCE, data[i]);
	  };
	  uart_putc_raw(PICO_PC_UART_INSTANCE, '*');

	  sprintf(cksumHex, "%X",cksum);
	  count = strlen(cksumHex);
	  for(uint8_t i = 0; i < count; i++){
          // ★ 修正: USART_Tx -> uart_putc_raw ★
		uart_putc_raw(PICO_PC_UART_INSTANCE, cksumHex[i]);
	  };
	  uart_putc_raw(PICO_PC_UART_INSTANCE, '\n');
}//--------------------------------------------------------------------------------------------

// 元の TX_IRQHandler は何もしないので削除または無視
//--------------------------------------------------------------------------------------------
// UART0 受信割り込みハンドラ (PC通信用)
//--------------------------------------------------------------------------------------------
static void mad_uart0_rx_irq_handler(void)
{
    // 受信バッファが空になるまで読み出す
    g_uart1_irq_count++;
    while (uart_is_readable(PICO_PC_UART_INSTANCE)) {
        char ch = uart_getc(PICO_PC_UART_INSTANCE); 
        
        mad_UART0_RX_BUF.empty = false;

        // オーバーフロー/エラーなら処理を停止
        if(mad_UART0_RX_BUF.overflow == true) return; 

        // データコピー
        mad_UART0_RX_BUF.data[mad_UART0_RX_BUF.index] = ch; 

        // 最初の文字が 'C' でなければエラーとする元のロジック
        if(mad_UART0_RX_BUF.data[0] != 'C'){ 
            mad_UART0_RX_BUF.overflow = true;
            mad_USART0_RxStop(); // 受信停止
            return;
        }

        mad_UART0_RX_BUF.index++;
        
        // 4文字受信完了
        if(mad_UART0_RX_BUF.index == 4){
            // チェックサムOKなら Enter カウンタを増やす
            // (注: mad_USART0_RxCkSumCheck4char の実装が必要です)
            if(mad_USART0_RxCkSumCheck4char() == true){
                mad_USART0_RxStop();
                mad_UART0_RX_BUF.enter++;
            }
            else{
                mad_USART0_RxStop();
                mad_UART0_RX_BUF.overflow = true;
                return;
            }
        }
        
        // 4文字超えでオーバーフロー
        if(mad_UART0_RX_BUF.index > 4){
            mad_USART0_RxStop();
            mad_UART0_RX_BUF.overflow = true;
            return;
        }
    }
}
//--------------------------------------------------------------------------------------------
// (バッファオーバーランとIRQストームを両方防ぐ v3 ロジック)
//--------------------------------------------------------------------------------------------
static void mad_uart1_rx_irq_handler(void)
{
    // (デバッグ用カウンタ - 必要ならコメント解除)
    // g_uart1_irq_count++;

    // FIFOが空になるまでデータを読み出し続ける
    while (uart_is_readable(PICO_GPS_UART_INSTANCE)) {
        
        // (1) データをFIFOから読み出す (IRQフラグをクリアするために必須)
        char ch = uart_getc(PICO_GPS_UART_INSTANCE); 

        // (2) 現在のインデックスが、バッファの *サイズ未満* かどうかをチェック
        // (sizeof(mad_UART1_RX_BUF.data) は 2048)
        if (mad_UART1_RX_BUF.index < sizeof(mad_UART1_RX_BUF.data)) {
            
            // (3) 安全な場合のみ、バッファにデータを書き込む
            mad_UART1_RX_BUF.data[mad_UART1_RX_BUF.index] = ch;
            mad_UART1_RX_BUF.empty = false;
        
            if (ch == 0x0a) // '\n' (LF)
                mad_UART1_RX_BUF.enter ++;
    
            // (4) データを書き込んだ後にインデックスを進める
            mad_UART1_RX_BUF.index++; 
        
        } else {
            // (5) インデックスがバッファサイズに達している (>= 2048)
            //     これはオーバーフロー。フラグを立てる。
            //     データは書き込まない (捨てる)。
            mad_UART1_RX_BUF.overflow = true;
        }
        
        // (ch は (1) で読み出し済みなので、ループが回る限り IRQ ストームは発生しない)
    }
}