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

// ADXL355のデフォルトI2Cスレーブアドレス
// MISO/ASELピンがLOWの場合: 0x1D
// 7ビットアドレスは0x1D [cite: 1548]
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

#ifdef i2c_default

// ADXL355のデフォルトODR/LPF設定 (4000Hz/1000Hz)に設定し、測定モードに移行
static void adxl355_setup() {
    uint8_t buf[2];
    uint8_t chip_id;

    // 1. デバイスIDの確認
    buf[0] = ADXL355_PARTID;
    i2c_write_blocking(i2c_default, addr, buf, 1, true); // trueでバス制御を維持
    i2c_read_blocking(i2c_default, addr, &chip_id, 1, false);

    if (chip_id != 0xED) { // ADXL355のデバイスIDは0xED [cite: 1786]
        printf("Error: ADXL355 PARTID mismatch! Found 0x%02X, Expected 0xED\n", chip_id);
        // エラー処理（ここでは続行）
    } else {
        printf("ADXL355 detected (PARTID: 0x%02X)\n", chip_id);
    }
    
    // 2. スタンバイモード(Standby=1)に設定 - 設定変更はスタンバイ中に行う必要がある [cite: 1764, 1956]
    // POWER_CTL (0x2D) レジスタに0x01 (Standby=1) を書き込み
    buf[0] = ADXL355_POWER_CTL;
    buf[1] = 0x01; 
    i2c_write_blocking(i2c_default, addr, buf, 2, false);
    sleep_ms(10); 

    // 3. 加速度レンジを±2gに設定
    // Rangeレジスタ (0x2C) のリセット値は0x81 (I2C_HS=1, Range=01: ±2g) [cite: 1943, 1945]
    // ここではリセット値と同じく0x81 (±2g)に設定
    buf[0] = ADXL355_RANGE;
    buf[1] = 0x81; 
    i2c_write_blocking(i2c_default, addr, buf, 2, false);
    
    // 4. フィルター設定 (ODR_LPF) を4000Hz/1000Hzに設定
    // Filterレジスタ (0x28) のリセット値は0x00 (HPF_CORNER=000, ODR_LPF=0000: 4000Hz/1000Hz) [cite: 1916, 1919]
    // ここではリセット値と同じく0x00に設定 (明示的な設定として)
    buf[0] = ADXL355_FILTER;
    buf[1] = 0x00;
    i2c_write_blocking(i2c_default, addr, buf, 2, false);

    // 5. 測定モード(Standby=0)に移行
    // POWER_CTL (0x2D) レジスタに0x00 (Standby=0) を書き込み
    buf[0] = ADXL355_POWER_CTL;
    buf[1] = 0x00; 
    i2c_write_blocking(i2c_default, addr, buf, 2, false); 
    sleep_ms(20); // ターンオン時間 <10ms [cite: 1779]
}

// ADXL355から生の加速度(X, Y, Z)と温度データを読み取る
static void adxl355_read_raw(int32_t accel[3], int16_t *temp) {
    uint8_t buffer[11]; // X:3バイト, Y:3バイト, Z:3バイト, Temp:2バイト

    // 1. 加速度データ (X, Y, Z) をレジスタ0x08 (XDATA3) から9バイト連続で読み出し
    // レジスタは自動インクリメントされるため、最初のレジスタアドレス0x08を指定すればOK [cite: 1557]
    uint8_t val = ADXL355_XDATA3;
    i2c_write_blocking(i2c_default, addr, &val, 1, true); // trueでバス制御を維持
    // XDATA3(0x08)〜XDATA1(0x0A), YDATA3(0x0B)〜YDATA1(0x0D), ZDATA3(0x0E)〜ZDATA1(0x10)の9バイト
    i2c_read_blocking(i2c_default, addr, buffer, 9, false); // False - バス制御を解放

    // 2. 20ビットの加速度データを32ビット整数に変換（左詰め）
    // データは左詰め、2の補数形式 [cite: 1551, 1823]
    // XDATA3: D19-D12, XDATA2: D11-D4, XDATA1: D3-D0 + 4 reserved bits (0x0Aのビット[3:0]はReserved) [cite: 1837]
    
    // X軸: buffer[0], buffer[1], buffer[2]
    accel[0] = (int32_t)((buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8));
    accel[0] = accel[0] >> 12; // 20ビットデータなので、12ビット右シフト (左詰め2の補数形式を考慮)

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
    // TEMP2: D11-D8 (ビット[3:0]), TEMP1: D7-D0 [cite: 1814]
    *temp = (buffer[0] << 8) | buffer[1];
    *temp = *temp & 0x0FFF; // 12ビットデータなので上位4ビットをマスク (TEMP2の上位4ビットはReserved) [cite: 1818]
    // データは符号なし (Unsigned) [cite: 1552, 1813]
}

// 温度データをLSB値から摂氏に変換するヘルパー関数
float convert_temp_to_c(int16_t raw_temp) {
    // データシートによると、公称切片は25°Cで1885 LSB、公称傾きは-9.05 LSB/°C [cite: 1813]
    // T_raw = T_25C + Scale_Factor * (T - 25)
    // T = 25 + (T_raw - T_25C) / Scale_Factor
    // T = 25 + (raw_temp - 1885) / (-9.05)
    
    // T_25C = 1885 LSB (Typ.)
    // Scale_Factor = -9.05 LSB/°C (Typ.)
    return 25.0f + ((float)raw_temp - 1885.0f) / -9.05f;
}

// 加速度データをLSB値からgに変換するヘルパー関数
float convert_accel_to_g(int32_t raw_accel) {
    // ±2gレンジでの公称感度 (Scale Factor) は 3.9 µg/LSB 
    // 1 g = 1,000,000 µg
    // 1 LSB = 3.9 µg/LSB = 3.9e-6 g/LSB
    
    // 256,000 LSB/g がTypical (±2gレンジ) 
    // Scale Factor = 1 / 256000 LSB/g = 3.90625e-6 g/LSB
    return (float)raw_accel * (1.0f / 256000.0f);
}

#endif

int main() {
    stdio_init_all();
#if !defined(i2c_default) || !defined(PICO_DEFAULT_I2C_SDA_PIN) || !defined(PICO_DEFAULT_I2C_SCL_PIN)
    #warning i2c/adxl355_i2c example requires a board with I2C pins
    puts("Default I2C pins were not defined");
    return 0;
#else
    printf("Hello, ADXL355! Setting up and reading raw data...\n");

    // I2C0を400kHzで初期化
    i2c_init(i2c_default, 400 * 1000); // 400kHz Fast Mode (デフォルト)
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));

    adxl355_setup();

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
        printf("Temp. = %.2f C\n", convert_temp_to_c(temp_raw));

        sleep_ms(100); // 100msごとに読み出し (ODR: 4000Hzなので余裕あり)
    }
#endif
}