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

// --- UART 設定定数 (printf 用) ---
#define UART_ID     uart0
#define BAUD_RATE   115200
#define UART_TX_PIN 0
#define UART_RX_PIN 1

// --- 関数プロトタイプ ---
void MainLoop(void);

// --- メインエントリーポイント ---
int main(void)
{
    // --- 手動 UART stdio 初期化 ---

    // --- ここまで ---

    // GPIO 初期化 (Lチカや ADXL355 が使う前に必要)
    // mad_SYSTEM_INIT() を呼ぶ代わりに、mad_GPIO_Init() を直接呼ぶ
    // ※mad_SYSTEM_INIT() は stdio_init_all() を呼んでいたのでもう不要
    mad_GPIO_Init();

    // 念のため少し待つ
    sleep_ms(100);

	mad_SYSTEM_INIT();

    // printf("--- UART Initialized. Initializing ADXL355... ---\n");
    // fflush(stdout);

    // ★ ADXL355 初期化 ★
    mad_ADXL355_Init();

    // printf("--- ADXL355 Initialized! Entering MainLoop... ---\n");
    // fflush(stdout);

    // メインループへ
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
        fflush(stdout); // 出力を確実にターミナルに送る

    } // end while(1)
} // end MainLoop