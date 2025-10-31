//--------------------------------------------------------------------------------------------
// RP2350 (pico-sdk) 版 mad_adxl355.c (完全版 - Open Drain修正)
//--------------------------------------------------------------------------------------------
#include "hal/mad_adxl355.h"       // ヘッダファイル
#include "hardware/gpio.h"         // Pico SDK GPIO 関数
#include "pico/time.h"             // busy_wait_us, sleep_ms のために追加
#include "hal/rp2350_pin_config.h" // ピン定義
#include "hal/mad_gpio.h"          // mad_GPIO_Get/Set/Clr を使用
#include "hal/mad_timer.h"         // mad_TIMER1_WAIT_10ms, mad_TIMER1_INIT/ENABLE を使用
#include <stdio.h>                // printf 用

// ★★★ I2C タイミング調整用 (マイクロ秒) ★★★
// この値を 1, 2, 5, 10, 20 などに変えて試す
#define I2C_WAIT_US 3

// --- グローバル変数の定義 (ヘッダで extern 宣言) ---
uint32_t    ADXL355_BYTE_DATA[ADXL355_CH];
int32_t     ADXL_AXEL_DATA[ADXL355_CH][ADXL355_AXIS];
bool        mad_ADXL_355_ERROR_CH1 = false;
bool        mad_ADXL_355_ERROR_CH2 = false;
bool        mad_ADXL_355_ERROR_CH3 = false;
bool        mad_ADXL_355_ERROR_CH4 = false;

// --- mad_GPIO_Set/Clr で使うための mad_GPIO_PIN 構造体 ---
static mad_GPIO_PIN scl_mad_pin;
static mad_GPIO_PIN sda_mad_pins[ADXL355_CH]; // Index 0=CH1, 1=CH2, ... (mad_GPIO_I2C_SDA_0 に対応)

// --- ピン番号 ---
const uint scl_pin = PIN_I2C_SCL;
// 配列のインデックスとチャンネル番号を合わせる (CH1=Index 0, ...)
const uint sda_pins[ADXL355_CH] = { PIN_I2C_SDA_1, PIN_I2C_SDA_2, PIN_I2C_SDA_3, PIN_I2C_SDA_4 };

// --- mad_GPIO_PIN 構造体を初期化 ---
static void mad_adxl355_pin_struct_init() {
    scl_mad_pin.pin = scl_pin;
    // sda_mad_pins[0] が mad_GPIO_I2C_SDA_0 に対応するように設定
    sda_mad_pins[CH1].pin = sda_pins[CH1]; // CH1 = Index 0
    sda_mad_pins[CH2].pin = sda_pins[CH2]; // CH2 = Index 1
    sda_mad_pins[CH3].pin = sda_pins[CH3]; // CH3 = Index 2
    sda_mad_pins[CH4].pin = sda_pins[CH4]; // CH4 = Index 3
}

// --- Open Drain エミュレーション用ヘルパー関数 ---
static inline void drive_sda_low(uint pin) {
    if (pin == (uint32_t)-1) return; // 未割り当てピンは無視
    gpio_disable_pulls(pin);
    gpio_set_dir(pin, GPIO_OUT);
    gpio_put(pin, 0);
}
static inline void release_sda_high(uint pin) {
    if (pin == (uint32_t)-1) return; // 未割り当てピンは無視
    gpio_set_dir(pin, GPIO_IN);
    // gpio_pull_up(pin); // 外部プルアップがあるので通常不要
}

// --- Wait 関数修正 ---
void mad_ADXL355_Wait(void) {
    busy_wait_us(I2C_WAIT_US);
}

// --- SCL/SDA 操作関数 (Open Drain 版) ---
void mad_ADXL355_SCL_Set(void) { mad_GPIO_Set(scl_mad_pin); mad_ADXL355_Wait(); }
void mad_ADXL355_SCL_Clr(void) { mad_GPIO_Clr(scl_mad_pin); mad_ADXL355_Wait(); }

void mad_ADXL355_SDA_Set(void) { // High (Release)
    release_sda_high(sda_pins[CH1]); release_sda_high(sda_pins[CH2]);
    release_sda_high(sda_pins[CH3]); release_sda_high(sda_pins[CH4]);
    mad_ADXL355_Wait();
}
void mad_ADXL355_SDA_Clr(void) { // Low (Drive)
    drive_sda_low(sda_pins[CH1]); drive_sda_low(sda_pins[CH2]);
    drive_sda_low(sda_pins[CH3]); drive_sda_low(sda_pins[CH4]);
    mad_ADXL355_Wait();
}

// --- GPIO 方向設定関数は不要 ---
// void mad_ADXL355_SDA_DIR_Read(void) { /* 不要 */ }
// void mad_ADXL355_SDA_DIR_Write(void) { /* 不要 */ }

// --- 4ch Read (完成版) ---
void mad_ADXL355_4ch_Read(void) {
    uint8_t d1=0, d2=0, d3=0, d4=0;
    mad_ADXL355_SDA_Set(); // Read の前に SDA を Input (High/Release) に

    mad_ADXL355_SCL_Set(); if(gpio_get(sda_pins[CH1])) d1|=0x80; if(gpio_get(sda_pins[CH2])) d2|=0x80; if(gpio_get(sda_pins[CH3])) d3|=0x80; if(gpio_get(sda_pins[CH4])) d4|=0x80; mad_ADXL355_SCL_Clr();
    mad_ADXL355_SCL_Set(); if(gpio_get(sda_pins[CH1])) d1|=0x40; if(gpio_get(sda_pins[CH2])) d2|=0x40; if(gpio_get(sda_pins[CH3])) d3|=0x40; if(gpio_get(sda_pins[CH4])) d4|=0x40; mad_ADXL355_SCL_Clr();
    mad_ADXL355_SCL_Set(); if(gpio_get(sda_pins[CH1])) d1|=0x20; if(gpio_get(sda_pins[CH2])) d2|=0x20; if(gpio_get(sda_pins[CH3])) d3|=0x20; if(gpio_get(sda_pins[CH4])) d4|=0x20; mad_ADXL355_SCL_Clr();
    mad_ADXL355_SCL_Set(); if(gpio_get(sda_pins[CH1])) d1|=0x10; if(gpio_get(sda_pins[CH2])) d2|=0x10; if(gpio_get(sda_pins[CH3])) d3|=0x10; if(gpio_get(sda_pins[CH4])) d4|=0x10; mad_ADXL355_SCL_Clr();
    mad_ADXL355_SCL_Set(); if(gpio_get(sda_pins[CH1])) d1|=0x08; if(gpio_get(sda_pins[CH2])) d2|=0x08; if(gpio_get(sda_pins[CH3])) d3|=0x08; if(gpio_get(sda_pins[CH4])) d4|=0x08; mad_ADXL355_SCL_Clr();
    mad_ADXL355_SCL_Set(); if(gpio_get(sda_pins[CH1])) d1|=0x04; if(gpio_get(sda_pins[CH2])) d2|=0x04; if(gpio_get(sda_pins[CH3])) d3|=0x04; if(gpio_get(sda_pins[CH4])) d4|=0x04; mad_ADXL355_SCL_Clr();
    mad_ADXL355_SCL_Set(); if(gpio_get(sda_pins[CH1])) d1|=0x02; if(gpio_get(sda_pins[CH2])) d2|=0x02; if(gpio_get(sda_pins[CH3])) d3|=0x02; if(gpio_get(sda_pins[CH4])) d4|=0x02; mad_ADXL355_SCL_Clr();
    mad_ADXL355_SCL_Set(); if(gpio_get(sda_pins[CH1])) d1|=0x01; if(gpio_get(sda_pins[CH2])) d2|=0x01; if(gpio_get(sda_pins[CH3])) d3|=0x01; if(gpio_get(sda_pins[CH4])) d4|=0x01; mad_ADXL355_SCL_Clr();

    ADXL355_BYTE_DATA[CH1] = d1; ADXL355_BYTE_DATA[CH2] = d2;
    ADXL355_BYTE_DATA[CH3] = d3; ADXL355_BYTE_DATA[CH4] = d4;
}

// --- Start/Stop/ACK/NAK (Open Drain 版) ---
void mad_ADXL355_Start(void) {
    mad_ADXL355_SDA_Set(); mad_ADXL355_SCL_Set();
    mad_ADXL355_SDA_Clr(); mad_ADXL355_SCL_Clr();
}
void mad_ADXL355_Stop(void) {
    mad_ADXL355_SDA_Clr(); mad_ADXL355_SCL_Set();
    mad_ADXL355_SDA_Set();
}
void mad_ADXL355_ACK_Out(void) { // Master sends ACK
    mad_ADXL355_SCL_Clr(); mad_ADXL355_SDA_Clr(); // Drive Low for ACK
    mad_ADXL355_SCL_Set(); mad_ADXL355_SCL_Clr();
    mad_ADXL355_SDA_Set(); // Release SDA
}
void mad_ADXL355_NAK_Out(void) { // Master sends NAK
    mad_ADXL355_SCL_Clr(); mad_ADXL355_SDA_Set(); // Release High for NAK
    mad_ADXL355_SCL_Set(); mad_ADXL355_SCL_Clr();
}

// --- Write (Open Drain 版 + ACK チェック修正) ---
bool mad_ADXL355_Write(uint8_t DATA) {
    // printf("Write(0x%02X)\n", DATA); fflush(stdout); // Debug

    // ビット送信 (ループなしスタイル)
    if ((DATA >> 7) & 1) { mad_ADXL355_SDA_Set(); } else { mad_ADXL355_SDA_Clr(); } mad_ADXL355_SCL_Set(); mad_ADXL355_SCL_Clr();
    if ((DATA >> 6) & 1) { mad_ADXL355_SDA_Set(); } else { mad_ADXL355_SDA_Clr(); } mad_ADXL355_SCL_Set(); mad_ADXL355_SCL_Clr();
    if ((DATA >> 5) & 1) { mad_ADXL355_SDA_Set(); } else { mad_ADXL355_SDA_Clr(); } mad_ADXL355_SCL_Set(); mad_ADXL355_SCL_Clr();
    if ((DATA >> 4) & 1) { mad_ADXL355_SDA_Set(); } else { mad_ADXL355_SDA_Clr(); } mad_ADXL355_SCL_Set(); mad_ADXL355_SCL_Clr();
    if ((DATA >> 3) & 1) { mad_ADXL355_SDA_Set(); } else { mad_ADXL355_SDA_Clr(); } mad_ADXL355_SCL_Set(); mad_ADXL355_SCL_Clr();
    if ((DATA >> 2) & 1) { mad_ADXL355_SDA_Set(); } else { mad_ADXL355_SDA_Clr(); } mad_ADXL355_SCL_Set(); mad_ADXL355_SCL_Clr();
    if ((DATA >> 1) & 1) { mad_ADXL355_SDA_Set(); } else { mad_ADXL355_SDA_Clr(); } mad_ADXL355_SCL_Set(); mad_ADXL355_SCL_Clr();
    if ((DATA >> 0) & 1) { mad_ADXL355_SDA_Set(); } else { mad_ADXL355_SDA_Clr(); } mad_ADXL355_SCL_Set(); mad_ADXL355_SCL_Clr();

    // --- ACK Check (CH1 のみ確認し、NAK なら false を返す) ---
    mad_ADXL355_SDA_Set(); // Release SDA for Slave ACK
    mad_ADXL355_SCL_Set();
    mad_ADXL_355_ERROR_CH1 = false; mad_ADXL_355_ERROR_CH2 = false;
    mad_ADXL_355_ERROR_CH3 = false; mad_ADXL_355_ERROR_CH4 = false;
    bool nak_received_ch1 = false;
    // mad_GPIO_Get の引数は mad_GPIO_PIN 構造体
    if (mad_GPIO_Get(sda_mad_pins[CH1])) { mad_ADXL_355_ERROR_CH1 = true; nak_received_ch1 = true; }
    if (mad_GPIO_Get(sda_mad_pins[CH2])) { mad_ADXL_355_ERROR_CH2 = true; } // エラーフラグは記録
    if (mad_GPIO_Get(sda_mad_pins[CH3])) { mad_ADXL_355_ERROR_CH3 = true; } // エラーフラグは記録
    if (mad_GPIO_Get(sda_mad_pins[CH4])) { mad_ADXL_355_ERROR_CH4 = true; } // エラーフラグは記録
    mad_ADXL355_SCL_Clr();
    mad_ADXL355_SDA_Set(); // Release SDA after reading ACK
    return !nak_received_ch1; // ★ CH1 が NAK なら false を返す ★
}

// --- Read (CH1 のみ) ---
uint8_t mad_ADXL355_Read(void) {
    uint8_t data = 0x00;
    mad_ADXL355_SDA_Set(); // Input

    mad_ADXL355_SCL_Set(); if(mad_GPIO_Get(sda_mad_pins[CH1])) data |= 0x80; mad_ADXL355_SCL_Clr();
    mad_ADXL355_SCL_Set(); if(mad_GPIO_Get(sda_mad_pins[CH1])) data |= 0x40; mad_ADXL355_SCL_Clr();
    mad_ADXL355_SCL_Set(); if(mad_GPIO_Get(sda_mad_pins[CH1])) data |= 0x20; mad_ADXL355_SCL_Clr();
    mad_ADXL355_SCL_Set(); if(mad_GPIO_Get(sda_mad_pins[CH1])) data |= 0x10; mad_ADXL355_SCL_Clr();
    mad_ADXL355_SCL_Set(); if(mad_GPIO_Get(sda_mad_pins[CH1])) data |= 0x08; mad_ADXL355_SCL_Clr();
    mad_ADXL355_SCL_Set(); if(mad_GPIO_Get(sda_mad_pins[CH1])) data |= 0x04; mad_ADXL355_SCL_Clr();
    mad_ADXL355_SCL_Set(); if(mad_GPIO_Get(sda_mad_pins[CH1])) data |= 0x02; mad_ADXL355_SCL_Clr();
    mad_ADXL355_SCL_Set(); if(mad_GPIO_Get(sda_mad_pins[CH1])) data |= 0x01; mad_ADXL355_SCL_Clr();
    return data;
}

// --- BYTE_READ (エラーチェック強化版) ---
bool mad_ADXL355_BYTE_READ(uint32_t register_address, uint8_t *data) {
    mad_ADXL355_Start();
    if (!mad_ADXL355_Write(ADXL355_Write_cmd)) { mad_ADXL355_Stop(); return false; }
    if (!mad_ADXL355_Write((uint8_t)register_address)) { mad_ADXL355_Stop(); return false; }
    mad_ADXL355_Start(); // Repeated Start
    if (!mad_ADXL355_Write(ADXL355_Read_cmd)) { mad_ADXL355_Stop(); return false; }
    *data = mad_ADXL355_Read();
    mad_ADXL355_NAK_Out(); // Last byte -> NAK
    mad_ADXL355_Stop();
    return true;
}

// --- BYTE_WRITE (エラーチェック強化版) ---
bool mad_ADXL355_BYTE_WRITE(uint32_t register_address, uint8_t data) {
    mad_ADXL355_Start();
    if (!mad_ADXL355_Write(ADXL355_Write_cmd)) { mad_ADXL355_Stop(); return false; }
    if (!mad_ADXL355_Write((uint8_t)register_address)) { mad_ADXL355_Stop(); return false; }
    if (!mad_ADXL355_Write(data)) { mad_ADXL355_Stop(); return false; }
    mad_ADXL355_Stop();
    return true;
}

// --- GetData (ビットシフト結合 + 明示的符号拡張 + デバッグ printf) ---
// hal/mad_adxl355.c 内

// --- GetData (ビットシフト結合 + 明示的符号拡張 + デバッグ printf + 修正済み) ---
// hal/mad_adxl355.c 内

// --- GetData (ビットシフト結合 + 明示的符号拡張 + デバッグ printf + バグ修正版) ---
bool mad_ADXL355_GetData(void) {
    // 毎回データ配列をクリア (エラー値 0xFFFFF で埋める)
    for(int ch=0; ch<ADXL355_CH; ++ch) {
        for(int ax=0; ax<ADXL355_AXIS; ++ax) {
            ADXL_AXEL_DATA[ch][ax] = 0xFFFFF; // エラー時のデフォルト値
        }
    }

    // --- 通信開始シーケンス ---
    mad_ADXL355_Start();
    // デバイスアドレス (Write mode) 送信
    if(!mad_ADXL355_Write(ADXL355_Write_cmd)) {
        // printf("GetData Error: NAK on DevAddr Wr\n"); fflush(stdout); // Debug
        mad_ADXL355_Stop();
        return false; // CH1 から ACK がなければ失敗
    }
    // 読み出し開始レジスタアドレス送信
    if(!mad_ADXL355_Write(ADXL355_AXEL_DATA_START_ADR)) {
        // printf("GetData Error: NAK on RegAddr\n"); fflush(stdout); // Debug
        mad_ADXL355_Stop();
        return false; // CH1 から ACK がなければ失敗
    }
    // Repeated Start
    mad_ADXL355_Start();
    // デバイスアドレス (Read mode) 送信
    if(!mad_ADXL355_Write(ADXL355_Read_cmd)) {
        // printf("GetData Error: NAK on DevAddr Rd\n"); fflush(stdout); // Debug
        mad_ADXL355_Stop();
        return false; // CH1 から ACK がなければ失敗
    }

    // --- データ読み出し (9バイト x 4ch) ---
    // ★★★ 軸ごとに生データを格納する配列を用意 ★★★
    uint8_t raw_x[ADXL355_CH], raw_y[ADXL355_CH], raw_z[ADXL355_CH]; // MSB 用
    uint8_t raw_x_mid[ADXL355_CH], raw_y_mid[ADXL355_CH], raw_z_mid[ADXL355_CH]; // MID 用
    uint8_t raw_x_lsb[ADXL355_CH], raw_y_lsb[ADXL355_CH], raw_z_lsb[ADXL355_CH]; // LSB 用

    // --- X軸 (3バイト読み出し) ---
    mad_ADXL355_4ch_Read(); mad_ADXL355_ACK_Out(); for(int c=0; c<ADXL355_CH; c++) raw_x[c] = (uint8_t)ADXL355_BYTE_DATA[c];
    mad_ADXL355_4ch_Read(); mad_ADXL355_ACK_Out(); for(int c=0; c<ADXL355_CH; c++) raw_x_mid[c] = (uint8_t)ADXL355_BYTE_DATA[c];
    mad_ADXL355_4ch_Read(); mad_ADXL355_ACK_Out(); for(int c=0; c<ADXL355_CH; c++) raw_x_lsb[c] = (uint8_t)ADXL355_BYTE_DATA[c];
    // ★ デバッグ出力: 生データ (X軸) ★
    // printf("Raw CH1 X: 0x%02X 0x%02X 0x%02X | ", raw_x[CH1], raw_x_mid[CH1], raw_x_lsb[CH1]);

    // --- Y軸 (3バイト読み出し) ---
    mad_ADXL355_4ch_Read(); mad_ADXL355_ACK_Out(); for(int c=0; c<ADXL355_CH; c++) raw_y[c] = (uint8_t)ADXL355_BYTE_DATA[c];
    mad_ADXL355_4ch_Read(); mad_ADXL355_ACK_Out(); for(int c=0; c<ADXL355_CH; c++) raw_y_mid[c] = (uint8_t)ADXL355_BYTE_DATA[c];
    mad_ADXL355_4ch_Read(); mad_ADXL355_ACK_Out(); for(int c=0; c<ADXL355_CH; c++) raw_y_lsb[c] = (uint8_t)ADXL355_BYTE_DATA[c];
    // ★ デバッグ出力: 生データ (Y軸) ★
    // printf("Y: 0x%02X 0x%02X 0x%02X | ", raw_y[CH1], raw_y_mid[CH1], raw_y_lsb[CH1]);

    // --- Z軸 (3バイト読み出し) ---
    mad_ADXL355_4ch_Read(); mad_ADXL355_ACK_Out(); for(int c=0; c<ADXL355_CH; c++) raw_z[c] = (uint8_t)ADXL355_BYTE_DATA[c];
    mad_ADXL355_4ch_Read(); mad_ADXL355_ACK_Out(); for(int c=0; c<ADXL355_CH; c++) raw_z_mid[c] = (uint8_t)ADXL355_BYTE_DATA[c];
    mad_ADXL355_4ch_Read(); mad_ADXL355_NAK_Out(); for(int c=0; c<ADXL355_CH; c++) raw_z_lsb[c] = (uint8_t)ADXL355_BYTE_DATA[c];
    // ★ デバッグ出力: 生データ (Z軸) ★
    // printf("Z: 0x%02X 0x%02X 0x%02X\n", raw_z[CH1], raw_z_mid[CH1], raw_z_lsb[CH1]);
    fflush(stdout); // Raw データ表示確定

    // --- 通信終了 ---
    mad_ADXL355_Stop();

    // --- データ結合 & 符号拡張 ---
    for (int ch = 0; ch < ADXL355_CH; ++ch) {
        // エラーチェック (Write 中に NAK があったか - mad_ADXL_355_ERROR_CHx に記録されている)
        bool error_occured = (ch == CH1 && mad_ADXL_355_ERROR_CH1) ||
                             (ch == CH2 && mad_ADXL_355_ERROR_CH2) ||
                             (ch == CH3 && mad_ADXL_355_ERROR_CH3) ||
                             (ch == CH4 && mad_ADXL_355_ERROR_CH4);

        if (!error_occured) {
            // エラーがないチャンネルのデータを結合・符号拡張
            // X軸
            uint32_t temp_x = ((uint32_t)raw_x[ch] << 12) | // MSB [19:12]
                              ((uint32_t)raw_x_mid[ch] << 4)  | // MID [11:4]
                              ((uint32_t)raw_x_lsb[ch] >> 4);   // LSB [3:0] (下位4bit捨てる)
            if (temp_x & 0x80000) { // 符号拡張
                ADXL_AXEL_DATA[ch][AXIS_X] = (int32_t)(temp_x | 0xFFF00000);
            } else {
                ADXL_AXEL_DATA[ch][AXIS_X] = (int32_t)temp_x;
            }

            // Y軸
            uint32_t temp_y = ((uint32_t)raw_y[ch] << 12) | // ★ 修正: raw_y を使用 ★
                              ((uint32_t)raw_y_mid[ch] << 4)  |
                              ((uint32_t)raw_y_lsb[ch] >> 4);
            if (temp_y & 0x80000) { // 符号拡張
                ADXL_AXEL_DATA[ch][AXIS_Y] = (int32_t)(temp_y | 0xFFF00000);
            } else {
                ADXL_AXEL_DATA[ch][AXIS_Y] = (int32_t)temp_y;
            }

            // Z軸
            uint32_t temp_z = ((uint32_t)raw_z[ch] << 12) | // ★ 修正: raw_z を使用 ★
                              ((uint32_t)raw_z_mid[ch] << 4)  |
                              ((uint32_t)raw_z_lsb[ch] >> 4);
            if (temp_z & 0x80000) { // 符号拡張
                ADXL_AXEL_DATA[ch][AXIS_Z] = (int32_t)(temp_z | 0xFFF00000);
            } else {
                ADXL_AXEL_DATA[ch][AXIS_Z] = (int32_t)temp_z;
            }

        }
        // else 節は不要 (ループ開始時に 0xFFFFF で初期化済み)
    } // end for ch

    // ★ デバッグ出力: 最終値 (オプション) ★
    // printf("Final CH1: X=%ld Y=%ld Z=%ld\n",
    //        ADXL_AXEL_DATA[CH1][AXIS_X], ADXL_AXEL_DATA[CH1][AXIS_Y], ADXL_AXEL_DATA[CH1][AXIS_Z]);
    // fflush(stdout);

    return true; // 通信シーケンスが完了すれば true
}
void mad_ADXL355_Init(void) {
    mad_adxl355_pin_struct_init(); // mad_GPIO_PIN 構造体初期化

    // SCLピン初期化 (Push-Pull Output, Init High)
    gpio_init(scl_pin); gpio_set_dir(scl_pin, GPIO_OUT); gpio_put(scl_pin, 1);

    // SDAピン初期状態 (Input + PullUp/Release High)
    for (int ch = 0; ch < ADXL355_CH; ++ch) {
         gpio_init(sda_pins[ch]); // ★ gpio_init を呼んでおく
         release_sda_high(sda_pins[ch]);
    }

    mad_ADXL355_Stop(); // Send Stop condition first
    sleep_ms(1);        // ★ 電源投入後/Stop後の安定待ち時間 ★

    // printf("Attempting ADXL355 Init Sequence...\n"); fflush(stdout);

    // ★ Init 時の Write の戻り値をチェックし、失敗したら return する ★
    if (!mad_ADXL355_BYTE_WRITE(0x2F, 0x52)) {
        printf("Init Error: Reset failed (NAK)\n"); fflush(stdout);
        // ★ エラー発生時はタイマーを開始せずに終了 ★
        return;
    }
    busy_wait_ms(10); // ★ リセット後の待機時間 ★

#ifdef SPS100
    if (!mad_ADXL355_BYTE_WRITE(0x28, ODR_125HZ)) { printf("Init Error: ODR failed (NAK)\n"); fflush(stdout); return; }
#endif
#ifdef SPS500
    if (!mad_ADXL355_BYTE_WRITE(0x28, ODR_250HZ)) { printf("Init Error: ODR failed (NAK)\n"); fflush(stdout); return; }
#endif
    busy_wait_ms(1); // コマンド間の短い待機

    if (!mad_ADXL355_BYTE_WRITE(0x2D, 0x00)) { printf("Init Error: PowerCtrl failed (NAK)\n"); fflush(stdout); return; }
    busy_wait_ms(1);

    // printf("ADXL355 Init Sequence Sent SUCCESSFULLY.\n"); fflush(stdout); // ここまで到達すればコマンド送信は成功

    // タイマー設定 (Init が成功した場合のみ設定)
#ifdef SPS500
    mad_TIMER1_INIT(40000, 2);
#endif
#ifdef SPS100
    mad_TIMER1_INIT(40000, 10);
#endif
    // mad_TIMER1_ENABLE();
    // printf("Timer ENABLE complete.\n"); // main 関数で Init 完了後に出力
}
//--------------------------------------------------------------------------------------------