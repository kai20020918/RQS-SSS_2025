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
#include "rp2350_pin_config.h"

uint32_t SYSTEMCLOCK;
int data1,data2;
time_t	mad_SYSTEM_TIME;
//--------------------------------------------------------------------------------------------
void mad_SYSTEM_INIT(void)
{
    // stdio_init_all(); // ★ 削除 (手動初期化と重複するため) ★
    
    // --- 手動 UART stdio 初期化 (printf 用 - UART0) ---
    // CM4/PC との通信ボーレート (921600 bps)
    uint actual_baudrate = uart_init(PICO_PC_UART_INSTANCE, 921600);
    
    if (actual_baudrate > 0) {
        gpio_set_function(PIN_PC_UART_TX, GPIO_FUNC_UART);
        gpio_set_function(PIN_PC_UART_RX, GPIO_FUNC_UART);
        stdio_uart_init_full(PICO_PC_UART_INSTANCE, 921600, PIN_PC_UART_TX, PIN_PC_UART_RX);
    }
    // (printf はこの関数が完了した後から使用可能になる)

    // GPIO初期化
    mad_GPIO_Init();
    
    // 元の40MHzに相当する値を設定 (pico-sdk のクロック設定とは異なるため注意)
    SYSTEMCLOCK = 40000000; 

    // 元: halInit(); // Initialize the system clocks and other HAL components
    // RTCC、CMUなどの設定はRP2350のアーキテクチャでは不要なため削除
    // mad_FLASH_READ();
}//--------------------------------------------------------------------------------------------
// クロック/省電力関数の置き換え (RP2350の特性に合わせて簡略化/削除)
// RP2350はクロック変更が容易でないため、一部をダミー化またはメインクロックを維持
void mad_SYSTEM_LFXO(void)
{
    // LFXO（低周波外部オシレータ）に相当する動作
    // Pico SDKでは、クロックを低速モードに設定する関数に置き換える
    // (ここでは具体的なRP2350の省電力APIがないため、一旦ダミー化)
    // 実際に省電力モードに入る際は、SDKのAPIを使用する
    SYSTEMCLOCK = 32768; // LFXOの周波数 (32kHz) を設定
}
void mad_SYSTEM_HFXO(void)
{
    // HFXO（高周波外部オシレータ）に相当する動作 (通常動作クロックに戻す)
    // clock_set_sys_clock_khz(40000, true); // 40MHzに戻す
    SYSTEMCLOCK = 40000000;
    mad_GPIO_WakeUp(); 
}
void mad_SYSTEM_LFRCO(void) {} // 内部低速RCオシレータはダミー
void mad_SYSTEM_HFRCO(void) {} // 内部高速RCオシレータはダミー
//--------------------------------------------------------------------------------------------
void mad_SYSTEM_EM3(void)
{
    // EM3（ディープスリープモード）に相当する動作
    // mad_USART1_RxStop();
    // mad_GPS_OFF(); (実装が必要)
    // mad_RF_TX_STOP(); (実装が必要)
    
    mad_SYSTEM_LFXO(); 
    mad_GPIO_Sleep();
    
    // Pico SDK のディープスリープ関数に置き換える
    // deep sleep / wfe / wfi の API を使用する (ここではダミー)
    // __wfi(); 
}
//--------------------------------------------------------------------------------------------
// RTC/TIME 関数の移植
// RTCC_DateGet/RTCC_TimeGet などの Silabs 固有関数は、
// mad_SYSTEM_TIME（time_t 型）を直接操作するロジックに置き換え
//--------------------------------------------------------------------------------------------
void mad_SYSTEM_TIME_RTC2SYSTIME(void)
{
    // 元の RTCC レジスタ読み出しを削除し、GPS/time.h のロジックに任せるため、
    // この関数は一旦空にするか、mad_SYSTEM_TIME の内容を更新するロジックに置き換える
    // mad_SYSTEM_TIME は GPS または time.h の mktime で更新される
}

// SYSTIME2RTC などの関数も、レジスタ操作を伴う部分は削除またはダミー化
uint32_t mad_SYSTEM_TIME_SYSTIME2RTC_DATE(void)
{
    // tm形式をRTC月日に変換 -> BCDレジスタ操作は削除し、ダミーのBCD値を返す
    return 0x20251029;
}
uint32_t mad_SYSTEM_TIME_SYSTIME2RTC_TIME(void)
{
    // tm形式をRTC時分秒に変換 -> BCDレジスタ操作は削除し、ダミーのBCD値を返す
    return 0x153000;
}
// mad_SYSTEM_TIME_GPS2SYSTIME, mad_SYSTEM_TIME_ZDA2SYSTIME など time.h ベースの関数は、
// 依存ヘッダーを修正すればそのまま利用可能
//--------------------------------------------------------------------------------------------