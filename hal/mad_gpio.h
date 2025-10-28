// mad_gpio.h (RP2350版)
#ifndef _MAD_GPIO_H
#define _MAD_GPIO_H

#include "pico/stdlib.h"
#include "mad_timer.h" // mad_GPIO_LED_FLASH が mad_TIMER を使うため

// RP2350 (pico-sdk) ではピン番号だけで管理
typedef struct {
	uint32_t pin;
} mad_GPIO_PIN;

// 元の抽象ピン名の宣言 (これらは main.c などから参照される)
extern mad_GPIO_PIN	mad_GPIO_I2C_SCL;
extern mad_GPIO_PIN	mad_GPIO_I2C_SDA_0;
extern mad_GPIO_PIN	mad_GPIO_I2C_SDA_1;
extern mad_GPIO_PIN	mad_GPIO_I2C_SDA_2;
extern mad_GPIO_PIN	mad_GPIO_I2C_SDA_3;

extern mad_GPIO_PIN	mad_GPIO_GPS_ON;
extern mad_GPIO_PIN	mad_GPIO_GPS_1PPS;

extern mad_GPIO_PIN	mad_GPIO_OUT_1;
extern mad_GPIO_PIN	mad_GPIO_OUT_2;
extern mad_GPIO_PIN	mad_GPIO_OUT_3;
extern mad_GPIO_PIN	mad_GPIO_OUT_4;
extern mad_GPIO_PIN	mad_GPIO_OUT_5;
extern mad_GPIO_PIN	mad_GPIO_OUT_6;

extern mad_GPIO_PIN	mad_GPIO_LED_1;
extern mad_GPIO_PIN	mad_GPIO_LED_2;
extern mad_GPIO_PIN	mad_GPIO_LED_3;
extern mad_GPIO_PIN	mad_GPIO_LED_4;
extern mad_GPIO_PIN	mad_GPIO_LED_5;

extern mad_GPIO_PIN	mad_GPIO_CONTACT_IN;
extern mad_GPIO_PIN	mad_GPIO_SAMPLES_PULSE;

extern mad_GPIO_PIN	mad_GPIO_RX1; // GPS
extern mad_GPIO_PIN	mad_GPIO_TX1; // GPS
extern mad_GPIO_PIN	mad_GPIO_TX0; // PC
extern mad_GPIO_PIN	mad_GPIO_RX0; // PC

// 関数のプロトタイプ宣言 (インターフェースは変更なし)
void	mad_GPIO_Init(void);
void	mad_GPIO_Sleep(void);
void	mad_GPIO_WakeUp(void);

void	mad_GPIO_Set(mad_GPIO_PIN f);
void	mad_GPIO_Clr(mad_GPIO_PIN f);
bool	mad_GPIO_Get(mad_GPIO_PIN f);

// mode は pico-sdk の gpio_set_dir や gpio_pull_up 等のラッパーとして別途実装
// void	mad_GPIO_Mode(mad_GPIO_PIN IOPIN, GPIO_Mode_TypeDef mode);
void	mad_GPIO_LED_FLASH(void);

#endif