// mad_gpio.c (RP2350版)
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "mad_gpio.h"
#include "mad_timer.h"
#include "rp2350_pin_config.h" // 新規作成したピン定義

// 抽象ピン構造体の実体
mad_GPIO_PIN	mad_GPIO_I2C_SCL;
mad_GPIO_PIN	mad_GPIO_I2C_SDA_0;
mad_GPIO_PIN	mad_GPIO_I2C_SDA_1;
mad_GPIO_PIN	mad_GPIO_I2C_SDA_2;
mad_GPIO_PIN	mad_GPIO_I2C_SDA_3;
mad_GPIO_PIN	mad_GPIO_GPS_ON;
mad_GPIO_PIN	mad_GPIO_GPS_1PPS;
mad_GPIO_PIN	mad_GPIO_OUT_1;
mad_GPIO_PIN	mad_GPIO_OUT_2;
mad_GPIO_PIN	mad_GPIO_OUT_3;
mad_GPIO_PIN	mad_GPIO_OUT_4;
mad_GPIO_PIN	mad_GPIO_OUT_5;
mad_GPIO_PIN	mad_GPIO_OUT_6;
mad_GPIO_PIN	mad_GPIO_LED_1;
mad_GPIO_PIN	mad_GPIO_LED_2;
mad_GPIO_PIN	mad_GPIO_LED_3;
mad_GPIO_PIN	mad_GPIO_LED_4;
mad_GPIO_PIN	mad_GPIO_LED_5;
mad_GPIO_PIN	mad_GPIO_CONTACT_IN;
mad_GPIO_PIN	mad_GPIO_SAMPLES_PULSE;
mad_GPIO_PIN	mad_GPIO_RX1;
mad_GPIO_PIN	mad_GPIO_TX1;
mad_GPIO_PIN	mad_GPIO_TX0;
mad_GPIO_PIN	mad_GPIO_RX0;

// 全ピンのリスト (初期化処理を簡潔にするため)
// (注意: このリストは mad_gpio.h の extern 宣言の順序と一致させること)
static mad_GPIO_PIN* all_mad_pins[] = {
    &mad_GPIO_I2C_SCL, &mad_GPIO_I2C_SDA_0, &mad_GPIO_I2C_SDA_1, 
    &mad_GPIO_I2C_SDA_2, &mad_GPIO_I2C_SDA_3,
    &mad_GPIO_GPS_ON, &mad_GPIO_GPS_1PPS,
    &mad_GPIO_OUT_1, &mad_GPIO_OUT_2, &mad_GPIO_OUT_3, &mad_GPIO_OUT_4,
    &mad_GPIO_OUT_5, &mad_GPIO_OUT_6,
    &mad_GPIO_LED_1, &mad_GPIO_LED_2, &mad_GPIO_LED_3, &mad_GPIO_LED_4, &mad_GPIO_LED_5,
    &mad_GPIO_CONTACT_IN, &mad_GPIO_SAMPLES_PULSE,
    &mad_GPIO_RX1, &mad_GPIO_TX1, &mad_GPIO_TX0, &mad_GPIO_RX0
};
static const int all_mad_pins_count = sizeof(all_mad_pins) / sizeof(all_mad_pins[0]);

void mad_GPIO_Init(void)
{
    // === 抽象ピンとRP2350ピン番号のマッピング ===
    // (マッピング仮定) 元のSDA 0-3 -> 新しいSDA 1-4
    mad_GPIO_I2C_SCL.pin    = PIN_I2C_SCL;
    mad_GPIO_I2C_SDA_0.pin  = PIN_I2C_SDA_1; 
    mad_GPIO_I2C_SDA_1.pin  = PIN_I2C_SDA_2;
    mad_GPIO_I2C_SDA_2.pin  = PIN_I2C_SDA_3;
    mad_GPIO_I2C_SDA_3.pin  = PIN_I2C_SDA_4;

    // (マッピング仮定) 元のLED 1-4 -> 新しいLED
    mad_GPIO_LED_1.pin      = PIN_LED_CONNECT;
    mad_GPIO_LED_2.pin      = PIN_LED_OUT;
    mad_GPIO_LED_3.pin      = PIN_LED_POWER;
    mad_GPIO_LED_4.pin      = PIN_LED_ALERT;
    mad_GPIO_LED_5.pin      = PIN_LED_5_ONBOARD; // TODO: 割当確認

    // UARTピン (mad_usart.c での初期化用)
    mad_GPIO_TX0.pin = PIN_PC_UART_TX;
    mad_GPIO_RX0.pin = PIN_PC_UART_RX;
    mad_GPIO_TX1.pin = PIN_GPS_UART_TX;
    mad_GPIO_RX1.pin = PIN_GPS_UART_RX;

    // === 割当不明なピン (TODO: rp2350_pin_config.h で -1 以外を割り当ててください) ===
    mad_GPIO_GPS_ON.pin     = -1; // TODO: GPS電源ピン
    mad_GPIO_GPS_1PPS.pin   = PIN_GPS_1PPS;
    mad_GPIO_OUT_1.pin      = PIN_OUT_1;
    mad_GPIO_OUT_2.pin      = PIN_OUT_2;
    mad_GPIO_OUT_3.pin      = PIN_OUT_3;
    mad_GPIO_OUT_4.pin      = PIN_OUT_4;
    mad_GPIO_OUT_5.pin      = PIN_OUT_5;
    mad_GPIO_OUT_6.pin      = PIN_OUT_6;
    mad_GPIO_CONTACT_IN.pin = PIN_CONTACT_IN;
    mad_GPIO_SAMPLES_PULSE.pin = PIN_SAMPLES_PULSE;

    // === pico-sdk GPIO初期化 ===
    for (int i = 0; i < all_mad_pins_count; i++) {
        uint32_t pin = all_mad_pins[i]->pin;
        if (pin == (uint32_t)-1) continue; // -1 は未割り当てとしてスキップ

        // mad_adxl355.c (I2C) と mad_usart.c (UART) で
        // 個別に設定するため、ここでは初期化しない
        if (pin == PIN_I2C_SCL || pin == PIN_I2C_SDA_1 || pin == PIN_I2C_SDA_2 ||
            pin == PIN_I2C_SDA_3 || pin == PIN_I2C_SDA_4 ||
            pin == PIN_PC_UART_TX || pin == PIN_PC_UART_RX ||
            pin == PIN_GPS_UART_TX || pin == PIN_GPS_UART_RX) {
            continue;
        }

        gpio_init(pin);

        // 元のコードに基づき、入力/出力を設定
        if (pin == PIN_CONTACT_IN || pin == PIN_GPS_1PPS) {
            gpio_set_dir(pin, GPIO_IN);
            // 元のコードは InputPull (mad_GPIO_Init)
            gpio_pull_up(pin); // または gpio_pull_down(pin)
        } else {
            // LED, OUT, SAMPLES_PULSE など
            gpio_set_dir(pin, GPIO_OUT);
        }
    }

    // 元のコードの初期状態（LED全消灯、出力全OFF）
    mad_GPIO_Set(mad_GPIO_LED_1);
  	mad_GPIO_Set(mad_GPIO_LED_2);
  	mad_GPIO_Set(mad_GPIO_LED_3);
  	mad_GPIO_Set(mad_GPIO_LED_4);
  	mad_GPIO_Set(mad_GPIO_LED_5);
  	mad_GPIO_Set(mad_GPIO_OUT_1);
  	mad_GPIO_Set(mad_GPIO_OUT_2);
  	mad_GPIO_Set(mad_GPIO_OUT_3);
  	mad_GPIO_Set(mad_GPIO_OUT_4);
  	mad_GPIO_Set(mad_GPIO_OUT_5);
  	mad_GPIO_Set(mad_GPIO_OUT_6);
}

// Set/Clr/Get は pico-sdk のラッパーとして実装
void mad_GPIO_Set(mad_GPIO_PIN f) {
    if (f.pin != (uint32_t)-1) {
        gpio_put(f.pin, 1);
    }
}

void mad_GPIO_Clr(mad_GPIO_PIN f) {
    if (f.pin != (uint32_t)-1) {
        gpio_put(f.pin, 0);
    }
}

bool mad_GPIO_Get(mad_GPIO_PIN f) {
    if (f.pin != (uint32_t)-1) {
        return gpio_get(f.pin);
    }
    return false;
}

// TODO: mad_GPIO_Sleep/WakeUp は必要に応じて実装
void mad_GPIO_Sleep() {}
void mad_GPIO_WakeUp(void) {}

void mad_GPIO_LED_FLASH(void) {
	while(1){
		mad_TIMER1_WAIT_10ms(30);
		mad_GPIO_Set(mad_GPIO_LED_5);
		mad_TIMER1_WAIT_10ms(30);
		mad_GPIO_Clr(mad_GPIO_LED_5);
	}
}