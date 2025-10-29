// main.c (Lチカテスト専用 最小構成)

#include "pico/stdlib.h" // stdio_init_all のために追加
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// --- HAL Includes ---
#include "hal/mad_gpio.h"
#include "hal/mad_timer.h"
#include "hal/mad_system.h"

// --- 他のインクルード (Lチカには不要なため、すべてコメントアウト) ---
// #include "hal/mad_adxl355.h" 
// #include "hal/mad_usart.h"   
// #include "hal/mad_flash.h"   
// #include "mad_gps.h"         

// --- 関数プロトタイプ (Lチカに必要なものだけ) ---
void MainLoop(void);

// --- メインエントリーポイント ---
int main(void)
{
    // mad_SYSTEM_INIT() は、内部で stdio_init_all() と mad_GPIO_Init() を呼び出します
    mad_SYSTEM_INIT();
    
    // メインループを呼び出し
    MainLoop();
    
    return 0; // (ここには到達しないはず)
}

// --- メインループ (Lチカテスト) ---
void MainLoop(void)
{
    // これは元のコードの起動時LEDシーケンスです
    mad_TIMER1_WAIT_10ms(20);	mad_GPIO_Clr(mad_GPIO_LED_1);
    mad_TIMER1_WAIT_10ms(20);	mad_GPIO_Clr(mad_GPIO_LED_2);
    mad_TIMER1_WAIT_10ms(20);	mad_GPIO_Clr(mad_GPIO_LED_3);
    mad_TIMER1_WAIT_10ms(20);	mad_GPIO_Clr(mad_GPIO_LED_4);

    mad_TIMER1_WAIT_10ms(20);	mad_GPIO_Set(mad_GPIO_LED_1);
    mad_TIMER1_WAIT_10ms(20);	mad_GPIO_Set(mad_GPIO_LED_2);
    mad_TIMER1_WAIT_10ms(20);	mad_GPIO_Set(mad_GPIO_LED_3);
    mad_TIMER1_WAIT_10ms(20);	mad_GPIO_Set(mad_GPIO_LED_4);

    // これは、動作確認用の新しい「無限点滅ループ」です
    // while(1) {
    //     mad_TIMER1_WAIT_10ms(50); // 0.5秒 待機
    //     mad_GPIO_Set(mad_GPIO_LED_1); // LED 1 点灯
    //     mad_GPIO_Clr(mad_GPIO_LED_2); // LED 2 消灯
        
    //     mad_TIMER1_WAIT_10ms(50); // 0.5秒 待機
    //     mad_GPIO_Clr(mad_GPIO_LED_1); // LED 1 消灯
    //     mad_GPIO_Set(mad_GPIO_LED_2); // LED 2 点灯
    // }
}

// --- 元の main.c にあった他の関数 (GetTtime, CommandAnalsis, など) ---
// --- は、Lチカテストに不要なため、このファイルから「すべて削除」します ---