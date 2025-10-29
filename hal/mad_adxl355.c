//--------------------------------------------------------------------------------------------
//Copyright 2019 Mathematical Assist Design Laboratories, Inc.
//All Rights Reserved.
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
#include "hal/mad_adxl355.h" // ヘッダファイルは hal/ ディレクトリにある想定
#include "hardware/gpio.h"
#include "pico/time.h"
#include "hal/rp2350_pin_config.h" // hal/ ディレクトリにある想定
#include "hal/mad_gpio.h"
#include "hal/mad_timer.h"

#define I2C_WAIT_US 1 // 例: 1マイクロ秒 (要調整)

// グローバル変数の定義 (ヘッダで extern 宣言したもの)
uint32_t    ADXL355_BYTE_DATA[ADXL355_CH];
int32_t     ADXL_AXEL_DATA[ADXL355_CH][ADXL355_AXIS];
bool        mad_ADXL_355_ERROR_CH1 = false;
bool        mad_ADXL_355_ERROR_CH2 = false;
bool        mad_ADXL_355_ERROR_CH3 = false;
bool        mad_ADXL_355_ERROR_CH4 = false;

// --- mad_GPIO_Set/Clr で使うための mad_GPIO_PIN 構造体 ---
// (これらは mad_ADXL355_Init などで初期化する必要がある)
static mad_GPIO_PIN scl_mad_pin;
static mad_GPIO_PIN sda_mad_pins[ADXL355_CH]; // SDA_0 から SDA_3 に対応

// --- ピン番号 (pico-sdk 関数で直接使うため) ---
const uint scl_pin = PIN_I2C_SCL;
const uint sda_pins[ADXL355_CH] = {
    PIN_I2C_SDA_1, // CH1 (元のSDA_0に対応)
    PIN_I2C_SDA_2, // CH2 (元のSDA_1に対応)
    PIN_I2C_SDA_3, // CH3 (元のSDA_2に対応)
    PIN_I2C_SDA_4  // CH4 (元のSDA_3に対応)
};

// --- mad_GPIO_PIN 構造体を初期化するヘルパー関数 ---
static void mad_adxl355_pin_struct_init() {
    scl_mad_pin.pin = scl_pin;
    sda_mad_pins[CH1].pin = sda_pins[CH1]; // mad_GPIO_I2C_SDA_0 に相当
    sda_mad_pins[CH2].pin = sda_pins[CH2]; // mad_GPIO_I2C_SDA_1 に相当
    sda_mad_pins[CH3].pin = sda_pins[CH3]; // mad_GPIO_I2C_SDA_2 に相当
    sda_mad_pins[CH4].pin = sda_pins[CH4]; // mad_GPIO_I2C_SDA_3 に相当
}


//--------------------------------------------------------------------------------------------
// SCL ピン操作 (mad_GPIO_* と mad_WAIT_us を使用)
//--------------------------------------------------------------------------------------------
void    mad_ADXL355_SCL_Set(void)
{
    mad_GPIO_Set(scl_mad_pin); // mad_GPIO_Set を使用
    mad_WAIT_us(I2C_WAIT_US);
}
//--------------------------------------------------------------------------------------------
void    mad_ADXL355_SCL_Clr(void)
{
    mad_GPIO_Clr(scl_mad_pin); // mad_GPIO_Clr を使用
    mad_WAIT_us(I2C_WAIT_US);
}
//--------------------------------------------------------------------------------------------
// SDA ピン操作 (4ch 同時、mad_GPIO_* と mad_WAIT_us を使用)
//--------------------------------------------------------------------------------------------
void    mad_ADXL355_SDA_Set(void)
{
    mad_GPIO_Set(sda_mad_pins[CH1]); // mad_GPIO_Set を使用
    mad_GPIO_Set(sda_mad_pins[CH2]);
    mad_GPIO_Set(sda_mad_pins[CH3]);
    mad_GPIO_Set(sda_mad_pins[CH4]);
    mad_WAIT_us(I2C_WAIT_US);
}
//--------------------------------------------------------------------------------------------
void    mad_ADXL355_SDA_Clr(void)
{
    mad_GPIO_Clr(sda_mad_pins[CH1]); // mad_GPIO_Clr を使用
    mad_GPIO_Clr(sda_mad_pins[CH2]);
    mad_GPIO_Clr(sda_mad_pins[CH3]);
    mad_GPIO_Clr(sda_mad_pins[CH4]);
    mad_WAIT_us(I2C_WAIT_US);
}
//--------------------------------------------------------------------------------------------
// SDA ピン方向設定 (pico-sdk 関数を直接使用)
//--------------------------------------------------------------------------------------------
void    mad_ADXL355_SDA_DIR_Read(void)
{
    // 元のコードの gpioModeInputPull, 1 相当 (入力 + プルアップ有効)
    gpio_set_dir(sda_pins[CH1], GPIO_IN); gpio_pull_up(sda_pins[CH1]);
    gpio_set_dir(sda_pins[CH2], GPIO_IN); gpio_pull_up(sda_pins[CH2]);
    gpio_set_dir(sda_pins[CH3], GPIO_IN); gpio_pull_up(sda_pins[CH3]);
    gpio_set_dir(sda_pins[CH4], GPIO_IN); gpio_pull_up(sda_pins[CH4]);
}
//--------------------------------------------------------------------------------------------
void    mad_ADXL355_SDA_DIR_Write(void)
{
    // 元のコードの gpioModeWiredAndPullUp 相当
    // Pico SDK には直接 Open Drain がないため、Push-Pull出力として設定
    // (必要であれば gpio_set_oeover 等で調整)
    // gpio_init は mad_GPIO_Init で実行済みのはずなので不要
    gpio_set_dir(sda_pins[CH1], GPIO_OUT);
    gpio_set_dir(sda_pins[CH2], GPIO_OUT);
    gpio_set_dir(sda_pins[CH3], GPIO_OUT);
    gpio_set_dir(sda_pins[CH4], GPIO_OUT);
}
//--------------------------------------------------------------------------------------------
// 4ch 並列読み出し (元のレジスタアクセスを gpio_get に置き換え)
//--------------------------------------------------------------------------------------------
void    mad_ADXL355_4ch_Read(void)
{
    // 一時的にデータを格納する変数
    uint8_t data_ch1 = 0, data_ch2 = 0, data_ch3 = 0, data_ch4 = 0;

    mad_ADXL355_SDA_DIR_Read(); // SDAピンを入力に設定

    // 元のコードのスタイルに合わせてループを使わずに記述
    mad_ADXL355_SCL_Set();
    if (gpio_get(sda_pins[CH1])) data_ch1 |= 0x80;
    if (gpio_get(sda_pins[CH2])) data_ch2 |= 0x80;
    if (gpio_get(sda_pins[CH3])) data_ch3 |= 0x80;
    if (gpio_get(sda_pins[CH4])) data_ch4 |= 0x80;
    mad_ADXL355_SCL_Clr();

    mad_ADXL355_SCL_Set();
    if (gpio_get(sda_pins[CH1])) data_ch1 |= 0x40;
    if (gpio_get(sda_pins[CH2])) data_ch2 |= 0x40;
    if (gpio_get(sda_pins[CH3])) data_ch3 |= 0x40;
    if (gpio_get(sda_pins[CH4])) data_ch4 |= 0x40;
    mad_ADXL355_SCL_Clr();

    mad_ADXL355_SCL_Set();
    if (gpio_get(sda_pins[CH1])) data_ch1 |= 0x20;
    if (gpio_get(sda_pins[CH2])) data_ch2 |= 0x20;
    if (gpio_get(sda_pins[CH3])) data_ch3 |= 0x20;
    if (gpio_get(sda_pins[CH4])) data_ch4 |= 0x20;
    mad_ADXL355_SCL_Clr();

    mad_ADXL355_SCL_Set();
    if (gpio_get(sda_pins[CH1])) data_ch1 |= 0x10;
    if (gpio_get(sda_pins[CH2])) data_ch2 |= 0x10;
    if (gpio_get(sda_pins[CH3])) data_ch3 |= 0x10;
    if (gpio_get(sda_pins[CH4])) data_ch4 |= 0x10;
    mad_ADXL355_SCL_Clr();

    mad_ADXL355_SCL_Set();
    if (gpio_get(sda_pins[CH1])) data_ch1 |= 0x08;
    if (gpio_get(sda_pins[CH2])) data_ch2 |= 0x08;
    if (gpio_get(sda_pins[CH3])) data_ch3 |= 0x08;
    if (gpio_get(sda_pins[CH4])) data_ch4 |= 0x08;
    mad_ADXL355_SCL_Clr();

    mad_ADXL355_SCL_Set();
    if (gpio_get(sda_pins[CH1])) data_ch1 |= 0x04;
    if (gpio_get(sda_pins[CH2])) data_ch2 |= 0x04;
    if (gpio_get(sda_pins[CH3])) data_ch3 |= 0x04;
    if (gpio_get(sda_pins[CH4])) data_ch4 |= 0x04;
    mad_ADXL355_SCL_Clr();

    mad_ADXL355_SCL_Set();
    if (gpio_get(sda_pins[CH1])) data_ch1 |= 0x02;
    if (gpio_get(sda_pins[CH2])) data_ch2 |= 0x02;
    if (gpio_get(sda_pins[CH3])) data_ch3 |= 0x02;
    if (gpio_get(sda_pins[CH4])) data_ch4 |= 0x02;
    mad_ADXL355_SCL_Clr();

    mad_ADXL355_SCL_Set();
    if (gpio_get(sda_pins[CH1])) data_ch1 |= 0x01;
    if (gpio_get(sda_pins[CH2])) data_ch2 |= 0x01;
    if (gpio_get(sda_pins[CH3])) data_ch3 |= 0x01;
    if (gpio_get(sda_pins[CH4])) data_ch4 |= 0x01;
    mad_ADXL355_SCL_Clr();

    // 結果をグローバル変数に格納
    ADXL355_BYTE_DATA[CH1] = data_ch1;
    ADXL355_BYTE_DATA[CH2] = data_ch2;
    ADXL355_BYTE_DATA[CH3] = data_ch3;
    ADXL355_BYTE_DATA[CH4] = data_ch4;
}
//--------------------------------------------------------------------------------------------
// I2C Start Condition
//--------------------------------------------------------------------------------------------
void    mad_ADXL355_Start(void)
{
    mad_ADXL355_SDA_DIR_Write(); // SDAを出力に
    mad_ADXL355_SDA_Set();       // SDA=H
    mad_ADXL355_SCL_Set();       // SCL=H
    mad_ADXL355_SDA_Clr();       // SDA=L (SCLがHの間にSDAをLにする -> Start)
    mad_ADXL355_SCL_Clr();       // SCL=L
}
//--------------------------------------------------------------------------------------------
// I2C Stop Condition
//--------------------------------------------------------------------------------------------
void    mad_ADXL355_Stop(void)
{
    mad_ADXL355_SDA_DIR_Write(); // SDAを出力に
    mad_ADXL355_SDA_Clr();       // SDA=L
    mad_ADXL355_SCL_Set();       // SCL=H
    mad_ADXL355_SDA_Set();       // SDA=H (SCLがHの間にSDAをHにする -> Stop)
}
//--------------------------------------------------------------------------------------------
// I2C ACK Output (Master -> Slave)
//--------------------------------------------------------------------------------------------
void    mad_ADXL355_ACK_Out(void)
{
    mad_ADXL355_SDA_DIR_Write(); // SDAを出力に
    mad_ADXL355_SCL_Clr();       // SCL=L
    mad_ADXL355_SDA_Clr();       // SDA=L (ACK)
    mad_ADXL355_SCL_Set();       // SCL=H
    mad_ADXL355_SCL_Clr();       // SCL=L
}
//--------------------------------------------------------------------------------------------
// I2C NAK Output (Master -> Slave)
//--------------------------------------------------------------------------------------------
void    mad_ADXL355_NAK_Out(void)
{
    mad_ADXL355_SDA_DIR_Write(); // SDAを出力に
    mad_ADXL355_SCL_Clr();       // SCL=L
    mad_ADXL355_SDA_Set();       // SDA=H (NAK)
    mad_ADXL355_SCL_Set();       // SCL=H
    mad_ADXL355_SCL_Clr();       // SCL=L
}
//--------------------------------------------------------------------------------------------
// 1バイト書き込み + ACK確認 (元のスタイルを維持)
//--------------------------------------------------------------------------------------------
bool    mad_ADXL355_Write(uint8_t DATA)
{
    mad_ADXL355_SDA_DIR_Write(); // SDAを出力に

    // 元のコードに合わせてループを使わずに記述
    (((DATA >> 7) &1) == 1) ? mad_ADXL355_SDA_Set(): mad_ADXL355_SDA_Clr();
    mad_ADXL355_SCL_Set(); mad_ADXL355_SCL_Clr();
    (((DATA >> 6) &1) == 1) ? mad_ADXL355_SDA_Set(): mad_ADXL355_SDA_Clr();
    mad_ADXL355_SCL_Set(); mad_ADXL355_SCL_Clr();
    (((DATA >> 5) &1) == 1) ? mad_ADXL355_SDA_Set(): mad_ADXL355_SDA_Clr();
    mad_ADXL355_SCL_Set(); mad_ADXL355_SCL_Clr();
    (((DATA >> 4) &1) == 1) ? mad_ADXL355_SDA_Set(): mad_ADXL355_SDA_Clr();
    mad_ADXL355_SCL_Set(); mad_ADXL355_SCL_Clr();
    (((DATA >> 3) &1) == 1) ? mad_ADXL355_SDA_Set(): mad_ADXL355_SDA_Clr();
    mad_ADXL355_SCL_Set(); mad_ADXL355_SCL_Clr();
    (((DATA >> 2) &1) == 1) ? mad_ADXL355_SDA_Set(): mad_ADXL355_SDA_Clr();
    mad_ADXL355_SCL_Set(); mad_ADXL355_SCL_Clr();
    (((DATA >> 1) &1) == 1) ? mad_ADXL355_SDA_Set(): mad_ADXL355_SDA_Clr();
    mad_ADXL355_SCL_Set(); mad_ADXL355_SCL_Clr();
    (((DATA >> 0) &1) == 1) ? mad_ADXL355_SDA_Set(): mad_ADXL355_SDA_Clr();
    mad_ADXL355_SCL_Set(); mad_ADXL355_SCL_Clr();

    // ACK 確認
    mad_ADXL355_SDA_DIR_Read();  // SDAを入力に設定
    mad_ADXL355_SCL_Set();       // SCL=H (SlaveがSDAをLにするのを待つ)

    // 各チャンネルのエラーフラグをリセット
    mad_ADXL_355_ERROR_CH1 = false;
    mad_ADXL_355_ERROR_CH2 = false;
    mad_ADXL_355_ERROR_CH3 = false;
    mad_ADXL_355_ERROR_CH4 = false;
    bool nak_received = false; // いずれかのチャンネルでNAKを受信したか

    // 各SDAピンの状態を確認 (LならACK、HならNAK)
    if (mad_GPIO_Get(sda_mad_pins[CH1])) { // mad_GPIO_Get を使用
        mad_ADXL_355_ERROR_CH1 = true;
        nak_received = true;
    }
    if (mad_GPIO_Get(sda_mad_pins[CH2])) {
        mad_ADXL_355_ERROR_CH2 = true;
        nak_received = true;
    }
    if (mad_GPIO_Get(sda_mad_pins[CH3])) {
        mad_ADXL_355_ERROR_CH3 = true;
        nak_received = true;
    }
    if (mad_GPIO_Get(sda_mad_pins[CH4])) {
        mad_ADXL_355_ERROR_CH4 = true;
        nak_received = true;
    }

    mad_ADXL355_SCL_Clr(); // SCL=L
    
    // NAK を受信したら false を返す (元のコードにはなかったが、追加)
    // return !nak_received; 
    return true; // 元のコードは常に true を返していた
}
//--------------------------------------------------------------------------------------------
// 1バイト読み込み (元のコードは CH1/SDA_0 のみから読み取る実装)
// 元のスタイルを維持
//--------------------------------------------------------------------------------------------
uint8_t mad_ADXL355_Read(void)
{
    uint8_t data = 0x00;
    mad_ADXL355_SDA_DIR_Read(); // SDAを入力に

    // 元のコードに合わせてループを使わずに記述 (SDA_0 / CH1 のみ)
    mad_ADXL355_SCL_Set(); if(mad_GPIO_Get(sda_mad_pins[CH1]) == 1) data |= 0x80; mad_ADXL355_SCL_Clr();
    mad_ADXL355_SCL_Set(); if(mad_GPIO_Get(sda_mad_pins[CH1]) == 1) data |= 0x40; mad_ADXL355_SCL_Clr();
    mad_ADXL355_SCL_Set(); if(mad_GPIO_Get(sda_mad_pins[CH1]) == 1) data |= 0x20; mad_ADXL355_SCL_Clr();
    mad_ADXL355_SCL_Set(); if(mad_GPIO_Get(sda_mad_pins[CH1]) == 1) data |= 0x10; mad_ADXL355_SCL_Clr();
    mad_ADXL355_SCL_Set(); if(mad_GPIO_Get(sda_mad_pins[CH1]) == 1) data |= 0x08; mad_ADXL355_SCL_Clr();
    mad_ADXL355_SCL_Set(); if(mad_GPIO_Get(sda_mad_pins[CH1]) == 1) data |= 0x04; mad_ADXL355_SCL_Clr();
    mad_ADXL355_SCL_Set(); if(mad_GPIO_Get(sda_mad_pins[CH1]) == 1) data |= 0x02; mad_ADXL355_SCL_Clr();
    mad_ADXL355_SCL_Set(); if(mad_GPIO_Get(sda_mad_pins[CH1]) == 1) data |= 0x01; mad_ADXL355_SCL_Clr();

    // この後に ACK/NAK を Master が Slave に送る必要がある
    // mad_ADXL355_BYTE_READ では Stop の前に ACK/NAK を送っていないため、
    // シングルバイトリード専用の実装と思われる。
    return data;
}
//--------------------------------------------------------------------------------------------
// レジスタからの1バイト読み込み (ロジックは元のまま)
//--------------------------------------------------------------------------------------------
bool    mad_ADXL355_BYTE_READ(uint32_t register_address, uint8_t *data)
{
    mad_ADXL355_Start();
    if(mad_ADXL355_Write(ADXL355_Write_cmd) == false)      {mad_ADXL355_Stop(); return false;}
    if(mad_ADXL355_Write(register_address) == false)      {mad_ADXL355_Stop(); return false;}
    mad_ADXL355_Start(); // Repeated Start
    if(mad_ADXL355_Write(ADXL355_Read_cmd) == false)       {mad_ADXL355_Stop(); return false;}
    *data = mad_ADXL355_Read();
    // 最後のバイトなので Master は NAK を送るのが一般的だが、元のコードにはない
    // mad_ADXL355_NAK_Out();
    mad_ADXL355_Stop();
    return true;
}
//--------------------------------------------------------------------------------------------
// レジスタへの1バイト書き込み (ロジックは元のまま)
//--------------------------------------------------------------------------------------------
bool    mad_ADXL355_BYTE_WRITE(uint32_t register_address, uint8_t data)
{
    mad_ADXL355_Start();
    if(mad_ADXL355_Write(ADXL355_Write_cmd) == false)  {mad_ADXL355_Stop(); return false;}
    if(mad_ADXL355_Write(register_address) == false)  {mad_ADXL355_Stop(); return false;}
    if(mad_ADXL355_Write(data) == false)              {mad_ADXL355_Stop(); return false;}
    mad_ADXL355_Stop();
    return true;
}
//--------------------------------------------------------------------------------------------
// ADXL355 データ取得 (ロジックは元のまま)
//--------------------------------------------------------------------------------------------
bool	mad_ADXL355_GetData(void)
{
//AXXL_AXEL_DATA[CH][AXIS]に、本来の加速度値の16倍の値を格納する。
	mad_ADXL355_Start();
	if(mad_ADXL355_Write(ADXL355_Write_cmd) == false)
		{mad_ADXL355_Stop(); return false;}
	if(mad_ADXL355_Write(ADXL355_AXEL_DATA_START_ADR) == false)
		{mad_ADXL355_Stop(); return false;}
	mad_ADXL355_Start();
	if(mad_ADXL355_Write(ADXL355_Read_cmd) == false)
		{mad_ADXL355_Stop(); return false;}

	mad_ADXL355_4ch_Read();		mad_ADXL355_ACK_Out();
	ADXL_AXEL_DATA[CH1][AXIS_X] = ADXL355_BYTE_DATA[CH1] * 65536;
	ADXL_AXEL_DATA[CH2][AXIS_X] = ADXL355_BYTE_DATA[CH2] * 65536;
	ADXL_AXEL_DATA[CH3][AXIS_X] = ADXL355_BYTE_DATA[CH3] * 65536;
	ADXL_AXEL_DATA[CH4][AXIS_X] = ADXL355_BYTE_DATA[CH4] * 65536;
	mad_ADXL355_4ch_Read();		mad_ADXL355_ACK_Out();
	ADXL_AXEL_DATA[CH1][AXIS_X] += ADXL355_BYTE_DATA[CH1] * 256;
	ADXL_AXEL_DATA[CH2][AXIS_X] += ADXL355_BYTE_DATA[CH2] * 256;
	ADXL_AXEL_DATA[CH3][AXIS_X] += ADXL355_BYTE_DATA[CH3] * 256;
	ADXL_AXEL_DATA[CH4][AXIS_X] += ADXL355_BYTE_DATA[CH4] * 256;
	mad_ADXL355_4ch_Read();		mad_ADXL355_ACK_Out();
	ADXL_AXEL_DATA[CH1][AXIS_X] += ADXL355_BYTE_DATA[CH1];
	ADXL_AXEL_DATA[CH2][AXIS_X] += ADXL355_BYTE_DATA[CH2];
	ADXL_AXEL_DATA[CH3][AXIS_X] += ADXL355_BYTE_DATA[CH3];
	ADXL_AXEL_DATA[CH4][AXIS_X] += ADXL355_BYTE_DATA[CH4];

	mad_ADXL355_4ch_Read();		mad_ADXL355_ACK_Out();
	ADXL_AXEL_DATA[CH1][AXIS_Y] = ADXL355_BYTE_DATA[CH1] * 65536;
	ADXL_AXEL_DATA[CH2][AXIS_Y] = ADXL355_BYTE_DATA[CH2] * 65536;
	ADXL_AXEL_DATA[CH3][AXIS_Y] = ADXL355_BYTE_DATA[CH3] * 65536;
	ADXL_AXEL_DATA[CH4][AXIS_Y] = ADXL355_BYTE_DATA[CH4] * 65536;
	mad_ADXL355_4ch_Read();		mad_ADXL355_ACK_Out();
	ADXL_AXEL_DATA[CH1][AXIS_Y] += ADXL355_BYTE_DATA[CH1] * 256;
	ADXL_AXEL_DATA[CH2][AXIS_Y] += ADXL355_BYTE_DATA[CH2] * 256;
	ADXL_AXEL_DATA[CH3][AXIS_Y] += ADXL355_BYTE_DATA[CH3] * 256;
	ADXL_AXEL_DATA[CH4][AXIS_Y] += ADXL355_BYTE_DATA[CH4] * 256;
	mad_ADXL355_4ch_Read();		mad_ADXL355_ACK_Out();
	ADXL_AXEL_DATA[CH1][AXIS_Y] += ADXL355_BYTE_DATA[CH1];
	ADXL_AXEL_DATA[CH2][AXIS_Y] += ADXL355_BYTE_DATA[CH2];
	ADXL_AXEL_DATA[CH3][AXIS_Y] += ADXL355_BYTE_DATA[CH3];
	ADXL_AXEL_DATA[CH4][AXIS_Y] += ADXL355_BYTE_DATA[CH4];

	mad_ADXL355_4ch_Read();		mad_ADXL355_ACK_Out();
	ADXL_AXEL_DATA[CH1][AXIS_Z] = ADXL355_BYTE_DATA[CH1] * 65536;
	ADXL_AXEL_DATA[CH2][AXIS_Z] = ADXL355_BYTE_DATA[CH2] * 65536;
	ADXL_AXEL_DATA[CH3][AXIS_Z] = ADXL355_BYTE_DATA[CH3] * 65536;
	ADXL_AXEL_DATA[CH4][AXIS_Z] = ADXL355_BYTE_DATA[CH4] * 65536;
	mad_ADXL355_4ch_Read();		mad_ADXL355_ACK_Out();
	ADXL_AXEL_DATA[CH1][AXIS_Z] += ADXL355_BYTE_DATA[CH1] * 256;
	ADXL_AXEL_DATA[CH2][AXIS_Z] += ADXL355_BYTE_DATA[CH2] * 256;
	ADXL_AXEL_DATA[CH3][AXIS_Z] += ADXL355_BYTE_DATA[CH3] * 256;
	ADXL_AXEL_DATA[CH4][AXIS_Z] += ADXL355_BYTE_DATA[CH4] * 256;
	mad_ADXL355_4ch_Read();		mad_ADXL355_Stop();
	ADXL_AXEL_DATA[CH1][AXIS_Z] += ADXL355_BYTE_DATA[CH1];
	ADXL_AXEL_DATA[CH2][AXIS_Z] += ADXL355_BYTE_DATA[CH2];
	ADXL_AXEL_DATA[CH3][AXIS_Z] += ADXL355_BYTE_DATA[CH3];
	ADXL_AXEL_DATA[CH4][AXIS_Z] += ADXL355_BYTE_DATA[CH4];

	ADXL_AXEL_DATA[CH1][AXIS_X] = ADXL_AXEL_DATA[CH1][AXIS_X] >> 4;
	ADXL_AXEL_DATA[CH1][AXIS_Y] = ADXL_AXEL_DATA[CH1][AXIS_Y] >> 4;
	ADXL_AXEL_DATA[CH1][AXIS_Z] = ADXL_AXEL_DATA[CH1][AXIS_Z] >> 4;
	ADXL_AXEL_DATA[CH2][AXIS_X] = ADXL_AXEL_DATA[CH2][AXIS_X] >> 4;
	ADXL_AXEL_DATA[CH2][AXIS_Y] = ADXL_AXEL_DATA[CH2][AXIS_Y] >> 4;
	ADXL_AXEL_DATA[CH2][AXIS_Z] = ADXL_AXEL_DATA[CH2][AXIS_Z] >> 4;
	ADXL_AXEL_DATA[CH3][AXIS_X] = ADXL_AXEL_DATA[CH3][AXIS_X] >> 4;
	ADXL_AXEL_DATA[CH3][AXIS_Y] = ADXL_AXEL_DATA[CH3][AXIS_Y] >> 4;
	ADXL_AXEL_DATA[CH3][AXIS_Z] = ADXL_AXEL_DATA[CH3][AXIS_Z] >> 4;
	ADXL_AXEL_DATA[CH4][AXIS_X] = ADXL_AXEL_DATA[CH4][AXIS_X] >> 4;
	ADXL_AXEL_DATA[CH4][AXIS_Y] = ADXL_AXEL_DATA[CH4][AXIS_Y] >> 4;
	ADXL_AXEL_DATA[CH4][AXIS_Z] = ADXL_AXEL_DATA[CH4][AXIS_Z] >> 4;

	return true;
}//--------------------------------------------------------------------------------------------
// ADXL355 初期化
//--------------------------------------------------------------------------------------------
void    mad_ADXL355_Init(void)
{
    // mad_GPIO_PIN 構造体の初期化 (最初に一度だけ実行)
    mad_adxl355_pin_struct_init();

    // SCLピンの初期化 (mad_GPIO_Init で OUTPUT になっている前提)
    // 元のコード: mad_GPIO_Mode(mad_GPIO_I2C_SCL, gpioModeWiredAndPullUp);
    // Pico SDK では Open Drain が直接ないので、Push-Pull のまま使う
    // (I2Cでは通常、外部プルアップ抵抗が必要)
    // 必要であれば gpio_init / gpio_set_dir をここで行う
    gpio_init(scl_pin);
    gpio_set_dir(scl_pin, GPIO_OUT);

    // SDAピンの初期化 (mad_GPIO_Init で OUTPUT になっている前提)
    // DIR_Write で方向は設定されるのでここでは何もしない
    
    // 最初に Stop Condition を送っておく
    mad_ADXL355_Stop();

    // ADXL355 ソフトウェアリセット
    mad_ADXL355_BYTE_WRITE(0x2F, 0x52);
    mad_TIMER1_WAIT_10ms(1); // リセット待機 (時間は要調整)

    // Output Data Rate (ODR) と Filter 設定
#ifdef  SPS100
    // mad_ADXL355_BYTE_WRITE(0x28, ODR_62R5HZ); mad_TIMER1_WAIT_10ms(1);
    mad_ADXL355_BYTE_WRITE(0x28, ODR_125HZ); mad_TIMER1_WAIT_10ms(1);
#endif
#ifdef  SPS500
    mad_ADXL355_BYTE_WRITE(0x28, ODR_250HZ); mad_TIMER1_WAIT_10ms(1);
#endif

    // Power Control: Measurement Mode
    mad_ADXL355_BYTE_WRITE(0x2D, 0x00);
    mad_TIMER1_WAIT_10ms(1);

    // タイマー設定 (サンプリング周期用)
#ifdef  SPS500
    mad_TIMER1_INIT(40000, 2); // 2ms周期 (500Hz)
#endif
#ifdef  SPS100
    mad_TIMER1_INIT(40000, 10); // 10ms周期 (100Hz)
#endif

    // タイマー開始 (main ループで使用)
    mad_TIMER1_ENABLE();
}
//--------------------------------------------------------------------------------------------