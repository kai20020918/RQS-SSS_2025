/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * * Modified for ADXL355 (Digital Output Accelerometer)
 */

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "hardware/uart.h" // UARTを使用するために追加

// ADXL355のデフォルトI2Cスレーブアドレス
// MISO/ASELピンがLOWの場合: 0x1D
static int addr = 0x1D;

// ADXL355 レジスタマップ (一部抜粋)
#define ADXL355_PARTID          0x02 // デバイスID (0xED)
#define ADXL355_STATUS          0x04 // ステータス
#define ADXL355_TEMP2           0x06 // 温度データ (MSB)
#define ADXL355_TEMP1           0x07 // 温度データ (LSB)
#define ADXL355_XDATA3          0x08 // X軸加速度データ (MSB)
#define ADXL355_FILTER          0x28 // フィルタ設定 (ODR/LPF)
#define ADXL355_RANGE           0x2C // I2Cスピード、レンジ設定
#define ADXL355_POWER_CTL       0x2D // パワーコントロール

// --- UARTピン設定 ---
// デフォルトのUART0を使用し、標準ピンを使用 (GP0/GP1) します。
// 必要に応じてこれらのピンを変更できます。
#define UART_ID uart0
#define BAUD_RATE 115200
#define UART_TX_PIN 0 // 例: GPIO0
#define UART_RX_PIN 1 // 例: GPIO1

// --- I2Cピン設定 ---
// ユーザー指定のGPIO7 (SCL) と GPIO8 (SDA) を使用
const uint SDA_PIN = 8; 
const uint SCL_PIN = 7; 

#ifdef i2c_default

// ADXL355のデフォルトODR/LPF設定 (4000Hz/1000Hz)に設定し、測定モードに移行
static void adxl355_setup() {
    uint8_t buf[2];
    uint8_t chip_id;

    // 1. デバイスIDの確認
    buf[0] = ADXL355_PARTID;
    i2c_write_blocking(i2c_default, addr, buf, 1, true); // trueでバス制御を維持
    i2c_read_blocking(i2c_default, addr, &chip_id, 1, false);

    if (chip_id != 0xED) { // ADXL355のデバイスIDは0xED 
        printf("Error: ADXL355 PARTID mismatch! Found 0x%02X, Expected 0xED\n", chip_id);
        // エラー処理（ここでは続行）
    } else {
        printf("ADXL355 detected (PARTID: 0x%02X)\n", chip_id);
    }
    
    // 2. スタンバイモード(Standby=1)に設定 - 設定変更はスタンバイ中に行う必要がある
    // POWER_CTL (0x2D) レジスタに0x01 (Standby=1) を書き込み
    buf[0] = ADXL355_POWER_CTL;
    buf[1] = 0x01; 
    i2c_write_blocking(i2c_default, addr, buf, 2, false);
    sleep_ms(10); 

    // 3. 加速度レンジを±2gに設定
    // Rangeレジスタ (0x2C) のリセット値は0x81 (I2C_HS=1, Range=01: ±2g)
    // ここではリセット値と同じく0x81 (±2g)に設定
    buf[0] = ADXL355_RANGE;
    buf[1] = 0x81; 
    i2c_write_blocking(i2c_default, addr, buf, 2, false);
    
    // 4. フィルター設定 (ODR_LPF) を4000Hz/1000Hzに設定
    // Filterレジスタ (0x28) のリセット値は0x00 (HPF_CORNER=000, ODR_LPF=0000: 4000Hz/1000Hz)
    // ここではリセット値と同じく0x00に設定 (明示的な設定として)
    buf[0] = ADXL355_FILTER;
    buf[1] = 0x00;
    i2c_write_blocking(i2c_default, addr, buf, 2, false);

    // 5. 測定モード(Standby=0)に移行
    // POWER_CTL (0x2D) レジスタに0x00 (Standby=0) を書き込み
    buf[0] = ADXL355_POWER_CTL;
    buf[1] = 0x00; 
    i2c_write_blocking(i2c_default, addr, buf, 2, false); 
    sleep_ms(20); // ターンオン時間 <10ms
}

// ADXL355から生の加速度(X, Y, Z)と温度データを読み取る
static void adxl355_read_raw(int32_t accel[3], int16_t *temp) {
    uint8_t buffer[11]; // X:3バイト, Y:3バイト, Z:3バイト, Temp:2バイト

    // 1. 加速度データ (X, Y, Z) をレジスタ0x08 (XDATA3) から9バイト連続で読み出し
    uint8_t val = ADXL355_XDATA3;
    i2c_write_blocking(i2c_default, addr, &val, 1, true); // trueでバス制御を維持
    i2c_read_blocking(i2c_default, addr, buffer, 9, false); // False - バス制御を解放

    // 2. 20ビットの加速度データを32ビット整数に変換（左詰め）
    // X軸: buffer[0], buffer[1], buffer[2]
    accel[0] = (int32_t)((buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8));
    accel[0] = accel[0] >> 12; // 20ビットデータなので、12ビット右シフト

    // Y軸: buffer[3], buffer[4], buffer[5]
    accel[1] = (int32_t)((buffer[3] << 24) | (buffer[4] << 16) | (buffer[5] << 8));
    accel[1] = accel[1] >> 12;

    // Z軸: buffer[6], buffer[7], buffer[8]
    accel[2] = (int32_t)((buffer[6] << 24) | (buffer[7] << 16) | (buffer[8] << 8));
    accel[2] = accel[2] >> 12;
    
    // 3. 温度データ (TEMP2, TEMP1) をレジスタ0x06から2バイト連続で読み出し
    val = ADXL355_TEMP2;
    i2c_write_blocking(i2c_default, addr, &val, 1, true); // trueでバス制御を維持
    i2c_read_blocking(i2c_default, addr, buffer, 2, false); // False - バス制御を解放

    // 4. 12ビットの温度データを16ビット整数に変換（右詰め）
    *temp = (buffer[0] << 8) | buffer[1];
    *temp = *temp & 0x0FFF; // 12ビットデータなので上位4ビットをマスク
}

// 温度データをLSB値から摂氏に変換するヘルパー関数
float convert_temp_to_c(int16_t raw_temp) {
    // T = 25 + (raw_temp - 1885) / (-9.05)
    return 25.0f + ((float)raw_temp - 1885.0f) / -9.05f;
}

// 加速度データをLSB値からgに変換するヘルパー関数
float convert_accel_to_g(int32_t raw_accel) {
    // 256,000 LSB/g がTypical (±2gレンジ)
    return (float)raw_accel * (1.0f / 256000.0f);
}

#endif

int main() {
    // ----------------------------------------------------
    // --- UART (printf) 初期化 ---
    // ----------------------------------------------------
    // 標準入出力(stdio)をUARTバックエンドに設定
    // stdio_init_all()の代わりに、stdio_usb_init()とstdio_uart_init()を明示的に使用
    // ここでは、USB出力を無効にして、UART出力を優先します。
    
    // UARTの初期化
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART); // 受信が不要でも設定しておく
    
    // stdioを初期化したUARTに割り当てる
    stdio_uart_init_full(UART_ID, BAUD_RATE, UART_TX_PIN, UART_RX_PIN);

    // ----------------------------------------------------
    // --- I2C 初期化 ---
    // ----------------------------------------------------
#if !defined(i2c_default) || !defined(PICO_DEFAULT_I2C_SDA_PIN) || !defined(PICO_DEFAULT_I2C_SCL_PIN)
    #warning i2c/adxl355_i2c example requires a board with I2C pins
    puts("Default I2C pins were not defined");
    return 0;
#else
    printf("Hello, ADXL355! Initializing UART and I2C...\n");

    // I2C0を400kHzで初期化
    i2c_init(i2c_default, 400 * 1000); // 400kHz Fast Mode (デフォルト)
    
    // SDA (GPIO08) と SCL (GPIO07) を I2C0 に割り当て
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C); // GPIO8 (SDA) に I2C 機能を設定
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C); // GPIO7 (SCL) に I2C 機能を設定
    
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    // I2Cピン情報をpicotoolに提供
    bi_decl(bi_2pins_with_func(SDA_PIN, SCL_PIN, GPIO_FUNC_I2C));

    // ADXL355の初期設定を実行
    adxl355_setup();
    // 意図しない2回目の adxl355_setup() の呼び出しを削除しました。
    
    int32_t acceleration[3]; 
    int16_t temp_raw;

    while (1) {
        adxl355_read_raw(acceleration, &temp_raw);

        // 生データ (LSB)
        printf("Raw Accel. X=%ld, Y=%ld, Z=%ld\n", acceleration[0], acceleration[1], acceleration[2]);
        printf("Raw Temp. = %d LSB\n", temp_raw);
        
        // 換算後の物理量 (g および ℃)
        printf("Accel. X=%.3f g, Y=%.3f g, Z=%.3f g\n", 
               convert_accel_to_g(acceleration[0]), 
               convert_accel_to_g(acceleration[1]), 
               convert_accel_to_g(acceleration[2]));
        printf("Temp. = %.2f C\n\n", convert_temp_to_c(temp_raw)); // 改行を追加して見やすく
        
        sleep_ms(100); // 100msごとに読み出し (ODR: 4000Hzなので余裕あり)
    }
#endif
}
