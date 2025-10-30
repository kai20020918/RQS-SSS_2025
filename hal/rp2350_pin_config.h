// rp2350_pin_config.h
#ifndef _RP2350_PIN_CONFIG_H
#define _RP2350_PIN_CONFIG_H

// #include "hardware/uart.h"

// === LEDピン ===
#define PIN_LED_CONNECT 12
#define PIN_LED_OUT     13
#define PIN_LED_POWER   14
#define PIN_LED_ALERT   15
// (元のLED_5の割当先が不明なため、オンボードLEDを仮に割り当て)
// RP2350にオンボードLEDがあればそのピン番号を、なければ -1 を指定
#define PIN_LED_5_ONBOARD -1 // (例: 25) 

// === I2C (ADXL355用カスタムI2C) ===
#define PIN_I2C_SCL     7
#define PIN_I2C_SDA_1   8  // (元のSDA_0)
#define PIN_I2C_SDA_2   9  // (元のSDA_1)
#define PIN_I2C_SDA_3   10 // (元のSDA_2)
#define PIN_I2C_SDA_4   11 // (元のSDA_3)

// === UART (PC) ===
#define PICO_PC_UART_INSTANCE   uart0
#define PIN_PC_UART_TX          0
#define PIN_PC_UART_RX          1

// === UART (GPS) ===
#define PICO_GPS_UART_INSTANCE  uart1
#define PIN_GPS_UART_TX         5
#define PIN_GPS_UART_RX         4

// === 割当不明なピン (TODO: 必要ならピン番号を割り当て) ===
#define PIN_OUT_1     -1
#define PIN_OUT_2     -1
#define PIN_OUT_3     -1
#define PIN_OUT_4     -1
#define PIN_OUT_5     -1
#define PIN_OUT_6     -1
#define PIN_CONTACT_IN -1
#define PIN_GPS_1PPS   -1
#define PIN_SAMPLES_PULSE -1

#endif