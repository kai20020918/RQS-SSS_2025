// mad_timer.c (RP2350版)
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "mad_timer.h"
#include "pico/time.h"

// グローバル変数 (元の .h ファイルで extern 宣言されている)
uint32_t	mad_TIMER1_CountValue;
uint32_t	mad_TIMER1_PostScale;
uint32_t	mad_TIMER1_PostScale_Default;
bool		mad_TIMER1_OverFlow = false; // main ループが参照するフラグ

// RP2350の repeating_timer ハンドル
static struct repeating_timer mad_timer_handle;
// repeating_timer の基本周期 (mad_TIMER1_INIT で設定)
static uint32_t g_base_period_us = 0;



//　新機関数の作成　Wait用

void mad_WAIT_us(uint32_t us){
    busy_wait_us(us);
}

// 



// 元の TIMER1_IRQHandler に相当するコールバック関数
static bool mad_timer_callback(struct repeating_timer *t) {
    mad_TIMER1_PostScale--;
	if(mad_TIMER1_PostScale == 0){
		mad_TIMER1_OverFlow = true; // main ループの while を抜ける
		mad_TIMER1_PostScale = mad_TIMER1_PostScale_Default;
	}
    return true; // タイマーを継続
}

// タイマーのパラメータを設定する (まだ開始はしない)
void mad_TIMER1_INIT(uint16_t CountValue, uint32_t PostScaleValue)
{
    // 元のコードは 40MHz クロックを基準にしていた
    // (mad_system.c -> SYSTEMCLOCK = 40000000;)
    const uint32_t SYSTEMCLOCK = 40000000; 

    // 元のタイマーの「1回の」周期をマイクロ秒で計算
    // (CountValue * 1,000,000) / SYSTEMCLOCK
    // (例: 40000 * 1000000 / 40000000 = 1000 us = 1ms)
    // (注: 元のコードの 10ms (100SPS) の場合:
    //  Count=40000, PostScale=10 -> 1ms * 10回 = 10ms)
    // (注: 元のコードの 2ms (500SPS) の場合:
    //  Count=40000, PostScale=2  -> 1ms * 2回 = 2ms)
    
    // …と思いましたが、mad_TIMER1_WAIT_10ms の実装を見ると
    // mad_TIMER1_INIT(SYSTEMCLOCK / 10000, count*100);
    // (40000000 / 10000) = 4000
    // (count * 100)
    // 周期は (4000 * (count*100)) / 40M = 0.01 * count = 10ms * count
    // つまり、CountValue が基本周期を決めているようです。
    
    // (再分析) mad_ADXL355_Init() の呼び出し:
    // 100SPS: mad_TIMER1_INIT(40000, 10);
    // 周期: (40000 * 10) / 40M = 0.01 sec = 10ms
    // 500SPS: mad_TIMER1_INIT(40000, 2);
    // 周期: (40000 * 2) / 40M = 0.002 sec = 2ms
    
    // 結論: repeating_timer の周期は (CountValue * PostScaleValue) [us]
    // ではなく、(CountValue / SYSTEMCLOCK) が基本周期 (us) で、
    // それが PostScaleValue 回呼ばれたら OverFlow。

    g_base_period_us = ((uint64_t)CountValue * 1000000) / SYSTEMCLOCK;

    mad_TIMER1_PostScale_Default = PostScaleValue;
	mad_TIMER1_ResetTimerCounter();
    mad_TIMER1_OverFlow = false;
}

// 同期（ブロッキング）待機
// (これは repeating_timer とは独立して動作)
void mad_TIMER1_WAIT_10ms(uint16_t count)
{
    sleep_ms(count * 10);
}

void mad_TIMER1_ResetTimerCounter(void)
{
	mad_TIMER1_PostScale = mad_TIMER1_PostScale_Default;
	mad_TIMER1_OverFlow = false;
}

// repeating_timer を開始
void mad_TIMER1_ENABLE(void)
{
    if (g_base_period_us > 0) {
        // 既存のタイマーが動いていればキャンセル
        cancel_repeating_timer(&mad_timer_handle);
        
        // 新しいタイマーを追加
        add_repeating_timer_us(g_base_period_us, mad_timer_callback, NULL, &mad_timer_handle);
    }
}

// repeating_timer を停止
void mad_TIMER1_DISABLE(void)
{
	cancel_repeating_timer(&mad_timer_handle);
}