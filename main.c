// main.c (ADXL355 + GPS ポーリングテスト用 - 修正版)

#include "pico/stdlib.h"
#include "hardware/uart.h" // uart_is_readable, uart_getc のために追加
#include "hardware/gpio.h"
#include "pico/sync.h"     // save_and_disable_interrupts
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// --- HAL Includes ---
#include "hal/mad_gpio.h"
#include "hal/mad_timer.h"
#include "hal/mad_system.h"
#include "hal/mad_adxl355.h"
#include "hal/rp2350_pin_config.h"
#include "hal/mad_gps.h"
#include "hal/mad_usart.h" // PICO_GPS_UART_INSTANCE を使うため

// --- 関数プロトタイプ ---
void MainLoop(void);

// --- メインエントリーポイント ---
int main(void)
{
    // ★ 1. SYSTEM_INIT を呼び出す ★
    // (内部で UART0 for printf @ 921600bps と mad_GPIO_Init が実行される)
	mad_SYSTEM_INIT();
    sleep_ms(100); 

    printf("--- System Init (UART0 + GPIO) Done ---\n");
    fflush(stdout);

    // ★ 2. ADXL355 初期化 ★
    mad_ADXL355_Init();
    printf("--- ADXL355 Init Done ---\n");
    fflush(stdout);

    // ★ 3. GPS (UART1) 初期化 ★
    mad_GPS_INIT(); // 内部で mad_USART1_INIT(9600) が呼ばれる
    printf("--- GPS Init (UART1 TX) Done ---\n");
    fflush(stdout);

    // ★ 4. PC コマンド受信 (UART0) → 一時停止 ★
    // mad_USART0_INIT(921600); 
    // printf("--- PC Command RX (UART0 IRQ) Started ---\n");
    // fflush(stdout);

    // ★ 5. GPS 受信 (UART1) 開始 → IRQ を使わないのでコメントアウト ★
    // mad_USART1_RxStart(); 
    // printf("--- GPS RX (UART1 IRQ) Started ---\n");
    // fflush(stdout);

    // メインループへ
    printf("Entering MainLoop... (Polling Mode)\n");
    fflush(stdout);
    MainLoop();

    return 0; // 到達しない
}

// --- メインループ (ADXL355 + GPS ポーリングテスト) ---
void MainLoop(void)
{
    // 起動時LEDシーケンス
    mad_TIMER1_WAIT_10ms(20); mad_GPIO_Clr(mad_GPIO_LED_1);
    mad_TIMER1_WAIT_10ms(20); mad_GPIO_Clr(mad_GPIO_LED_2);
    mad_TIMER1_WAIT_10ms(20); mad_GPIO_Clr(mad_GPIO_LED_3);
    mad_TIMER1_WAIT_10ms(20); mad_GPIO_Clr(mad_GPIO_LED_4);
    mad_TIMER1_WAIT_10ms(20); mad_GPIO_Set(mad_GPIO_LED_1);
    mad_TIMER1_WAIT_10ms(20); mad_GPIO_Set(mad_GPIO_LED_2);
    mad_TIMER1_WAIT_10ms(20); mad_GPIO_Set(mad_GPIO_LED_3);
    mad_TIMER1_WAIT_10ms(20); mad_GPIO_Set(mad_GPIO_LED_4);

    uint32_t loop_count = 0;
    char gps_char; // 1文字ずつ受信するバッファ

    // ★ 無限ループでデータを読み取り続ける ★
    while(1) {
        // 例: 200ms ごとに読み取り (5Hz)
        sleep_ms(200);
        loop_count++;

        // データを取得
        bool success = mad_ADXL355_GetData();

        if (success) {
            // ADXL355 データ表示
            printf("[%lu] CH1: X=%ld Y=%ld Z=%ld | CH2: X=%ld Y=%ld Z=%ld | CH3: X=%ld Y=%ld Z=%ld | CH4: X=%ld Y=%ld Z=%ld",
                   loop_count,
                   ADXL_AXEL_DATA[CH1][AXIS_X], ADXL_AXEL_DATA[CH1][AXIS_Y], ADXL_AXEL_DATA[CH1][AXIS_Z],
                   ADXL_AXEL_DATA[CH2][AXIS_X], ADXL_AXEL_DATA[CH2][AXIS_Y], ADXL_AXEL_DATA[CH2][AXIS_Z],
                   ADXL_AXEL_DATA[CH3][AXIS_X], ADXL_AXEL_DATA[CH3][AXIS_Y], ADXL_AXEL_DATA[CH3][AXIS_Z],
                   ADXL_AXEL_DATA[CH4][AXIS_X], ADXL_AXEL_DATA[CH4][AXIS_Y], ADXL_AXEL_DATA[CH4][AXIS_Z]);

            if (mad_ADXL_355_ERROR_CH1 || mad_ADXL_355_ERROR_CH2 || mad_ADXL_355_ERROR_CH3 || mad_ADXL_355_ERROR_CH4) {
                printf(" ERRORS: [%s%s%s%s]",
                       mad_ADXL_355_ERROR_CH1 ? "1" : "",
                       mad_ADXL_355_ERROR_CH2 ? "2" : "",
                       mad_ADXL_355_ERROR_CH3 ? "3" : "",
                       mad_ADXL_355_ERROR_CH4 ? "4" : "");
            }
            // printf("\n"); // ★ 改行を削除 ★
        } else {
            printf("[%lu] ADXL355 GetData Failed", loop_count);
            mad_GPIO_Set(mad_GPIO_LED_4); sleep_ms(50); mad_GPIO_Clr(mad_GPIO_LED_4);
        }

        printf(" | "); // GPS データとの区切り

        // --- ★ GPS データ受信確認 (正しいポーリング方式) ★ ---
        if (uart_is_readable(PICO_GPS_UART_INSTANCE)) {
            printf("GPS Data: ");
            // 読めるデータがなくなるまで読み出す
            while (uart_is_readable(PICO_GPS_UART_INSTANCE)) {
                gps_char = uart_getc(PICO_GPS_UART_INSTANCE);
                printf("%c", gps_char); // 1文字ずつ UART0 (CM4) に出力
            }
            // (NMEA センテンスは '\n' で改行されるので、ここでは改行しない)
        } else {
             printf("GPS Data: --- \n"); // データがない場合のみ改行
        }
        
        fflush(stdout); // 1ループ分の出力をまとめてフラッシュ
    } // end while(1)
} // end MainLoop