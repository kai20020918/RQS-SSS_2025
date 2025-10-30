// main.c (ADXL355 テスト用 - 手動 UART 初期化版)

#include "pico/stdlib.h"
#include "hardware/uart.h" // 手動初期化用
#include "hardware/gpio.h" // 手動初期化用 & Lチカ用
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// --- HAL Includes ---
#include "hal/mad_gpio.h"
#include "hal/mad_timer.h"
#include "hal/mad_system.h" // mad_SYSTEM_INIT はもう不要かも？
#include "hal/mad_adxl355.h" // ADXL355 を使うのでインクルード
#include "hal/rp2350_pin_config.h" // PIN_LED_CONNECT などを使うため
#include "hal/mad_gps.h"
#include "mad_usart.h"



// --- 関数プロトタイプ ---
void MainLoop(void);

// --- メインエントリーポイント ---
int main(void)
{
    // ★ 1. SYSTEM_INIT を呼び出す ★
    // (内部で UART0 for printf @ 921600bps と mad_GPIO_Init が実行される)
	mad_SYSTEM_INIT();
    sleep_ms(100); // UART 初期化待ち

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

    // ★ 4. PC コマンド受信 (UART0) 開始 ★
    mad_USART0_INIT(921600); // UART0 の受信割り込みのみ設定
    printf("--- PC Command RX (UART0 IRQ) Started ---\n");
    fflush(stdout);

    // ★ 5. GPS 受信 (UART1) 開始 ★
    mad_USART1_RxStart(); // UART1 の受信割り込みを有効化
    printf("--- GPS RX (UART1 IRQ) Started ---\n");
    fflush(stdout);

    // メインループへ
    printf("Entering MainLoop...\n");
    fflush(stdout);
    MainLoop();

    return 0; // 到達しない
}
// --- メインループ (ADXL355 データ読み取りテスト) ---
void MainLoop(void)
{
    // 起動時LEDシーケンス (デバッグ用に残しても良い)
    mad_TIMER1_WAIT_10ms(20); mad_GPIO_Clr(mad_GPIO_LED_1);
    mad_TIMER1_WAIT_10ms(20); mad_GPIO_Clr(mad_GPIO_LED_2);
    mad_TIMER1_WAIT_10ms(20); mad_GPIO_Clr(mad_GPIO_LED_3);
    mad_TIMER1_WAIT_10ms(20); mad_GPIO_Clr(mad_GPIO_LED_4);
    mad_TIMER1_WAIT_10ms(20); mad_GPIO_Set(mad_GPIO_LED_1);
    mad_TIMER1_WAIT_10ms(20); mad_GPIO_Set(mad_GPIO_LED_2);
    mad_TIMER1_WAIT_10ms(20); mad_GPIO_Set(mad_GPIO_LED_3);
    mad_TIMER1_WAIT_10ms(20); mad_GPIO_Set(mad_GPIO_LED_4);

    uint32_t loop_count = 0;

    // ★ 無限ループでデータを読み取り続ける ★
    while(1) {
        // 例: 200ms ごとに読み取り (5Hz)
        sleep_ms(200);
        loop_count++;

        // データを取得
        bool success = mad_ADXL355_GetData();

        if (success) {
            // 読み取ったデータを printf で出力 (例: CH1 と CH2 の X, Y, Z)
            // ADXL_AXEL_DATA は mad_adxl355.c で定義されたグローバル変数
            printf("[%lu] CH1: X=%ld Y=%ld Z=%ld | CH2: X=%ld Y=%ld Z=%ld | CH3: X=%ld Y=%ld Z=%ld | CH4: X=%ld Y=%ld Z=%ld",
                   loop_count,
                   ADXL_AXEL_DATA[CH1][AXIS_X], ADXL_AXEL_DATA[CH1][AXIS_Y], ADXL_AXEL_DATA[CH1][AXIS_Z],
                   ADXL_AXEL_DATA[CH2][AXIS_X], ADXL_AXEL_DATA[CH2][AXIS_Y], ADXL_AXEL_DATA[CH2][AXIS_Z],
				   ADXL_AXEL_DATA[CH3][AXIS_X], ADXL_AXEL_DATA[CH3][AXIS_Y], ADXL_AXEL_DATA[CH3][AXIS_Z],
				   ADXL_AXEL_DATA[CH4][AXIS_X], ADXL_AXEL_DATA[CH4][AXIS_Y], ADXL_AXEL_DATA[CH4][AXIS_Z]);
                   // 必要であれば CH3, CH4 も追加

            // エラーフラグも確認 (mad_ADXL355_Write 内でセットされる)
            if (mad_ADXL_355_ERROR_CH1 || mad_ADXL_355_ERROR_CH2 || mad_ADXL_355_ERROR_CH3 || mad_ADXL_355_ERROR_CH4) {
                printf(" ERRORS: [%s%s%s%s]",
                       mad_ADXL_355_ERROR_CH1 ? "1" : "",
                       mad_ADXL_355_ERROR_CH2 ? "2" : "",
                       mad_ADXL_355_ERROR_CH3 ? "3" : "",
                       mad_ADXL_355_ERROR_CH4 ? "4" : "");
            }
            printf("\n");

        } else {
            printf("[%lu] ADXL355 GetData Failed\n", loop_count);

            // エラー発生時は LED を点滅させるなどで通知
            mad_GPIO_Set(mad_GPIO_LED_4); // エラーLED (ALERT) 点灯
            sleep_ms(50);
            mad_GPIO_Clr(mad_GPIO_LED_4); // エラーLED 消灯
        }
        // fflush(stdout); // 出力を確実にターミナルに送る

        if(!mad_UART1_RX_BUF.empty){
            printf("GPS Data:");
            printf("%s",mad_UART1_RX_BUF.data);

            mad_USART1_RxBufClr();

        }else{
            printf("GPS Dat : ------\n");


        }

        fflush(stdout);

    } // end while(1)
} // end MainLoop