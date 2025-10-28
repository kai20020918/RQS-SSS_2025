//--------------------------------------------------------------------------------------------
//Copyright 2019 Mathematical Assist Design Laboratories, Inc.
//All Rights Reserved.
//--------------------------------------------------------------------------------------------
//#define raspberry
//#define armadillo
//#define debug

#define NoGPS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// #include "bsp.h"
// #include "hal-config.h"
// #include "hal_common.h"
// #include "gpiointerrupt.h"

// #include "rail_config.h"
// #include "rail.h"
// #include "rail_types.h"

// #include "em_chip.h"
// #include "em_cmu.h"
// #include "em_gpio.h"
// #include "em_emu.h"
// #include "em_timer.h"
// #include "em_rtcc.h"
// #include "emlib\em_vdac.h"

#include "hal/mad_gpio.h"
// #include "hal/mad_i2c.h"      // (これはまだ無いので、後でエラーになるか、コメントアウト)
#include "hal/mad_timer.h"
// #include "hal/mad_usart.h"    // (同上)
// #include "hal/mad_flash.h"    // (同上)
// #include "hal/mad_rtc.h"      // (同上)
#include "hal/mad_system.h"
#include "mad_gps.h" 
// #include "hal/mad_adxl355.h"  // (同上)
//#include "mad\mad_gpstx.h"
//#include "mad\mad_scope-tx.h"
//#include "koyo\user_function_internal_rtcc.h"
//#include "mad\mad_rf.h"
//#include "mad\mad_ss.h"
//#include "mad\mad_dac.h"
//#include "mad\mad_si5351a.h"
//#include "mad\mad_mma8451q.h"
//#include "mad\mad_m24m01.h"
//--------------------------------------------------------------------------------------------
void	SetMode(void);
void	MainLoop(void);
void	GetTxString(void);
void	GetTtime(void);
void	CommandAnalsis(void);

//--------------------------------------------------------------------------------------------
#define LFRCO_FREQUENCY                 32768
#define WAKEUP_INTERVAL_MS              3000
#define RTC_COUNT_BETWEEN_WAKEUP        (((LFRCO_FREQUENCY * WAKEUP_INTERVAL_MS) / 1000)-1)
//--------------------------------------------------------------------------------------------
uint32_t	DataInc;
char		TxChar[44];
bool		GPS_1PPS;				//GPSの1PPS信号をそのまま出力		H=true L=false
bool		Contact_In;				//接点入力の状態をそのまま出力		H-true L=false
bool		GPS_Active;				//過去1秒以内にGPSを受信していたら	true;
bool		GPS_1PPS_Enabled;		//1PPS検出フラグ　				タイマ割込み待ちの間に1PPSが来てればture;

unsigned int	GPS_Active_Count;		//GPS受信完了

#ifdef	SPS500
#define		GPS_Active_Count_Default 100
#else if 	SPS500
#define		GPS_Active_Count_Default 500
#endif

//--------------------------------------------------------------------------------------------
enum	stat{
	READY,
	RUN,
	ONCE
	}MeasureStatus;
//--------------------------------------------------------------------------------------------
void	SAMPLE_Pulse(void)
{
	mad_GPIO_Set(mad_GPIO_SAMPLES_PULSE);
	mad_GPIO_Set(mad_GPIO_SAMPLES_PULSE);
	mad_GPIO_Set(mad_GPIO_SAMPLES_PULSE);
	mad_GPIO_Set(mad_GPIO_SAMPLES_PULSE);
	mad_GPIO_Set(mad_GPIO_SAMPLES_PULSE);
	mad_GPIO_Set(mad_GPIO_SAMPLES_PULSE);
	mad_GPIO_Clr(mad_GPIO_SAMPLES_PULSE);
}
//--------------------------------------------------------------------------------------------
int main(void)
{
	mad_SYSTEM_INIT();
	mad_FLASH_DATA_INIT();		//
	MainLoop();
}
//--------------------------------------------------------------------------------------------
void	MainLoop(void)
{
#ifndef NoGPS
	//GPS受信開始
	mad_GPS_INIT();
	mad_GPS_BufClr();
	mad_USART1_RxStart();
#endif


/*	while(1){

	mad_TIMER1_WAIT_10ms(2);	mad_GPIO_Clr(mad_GPIO_LED_1);			//
	mad_TIMER1_WAIT_10ms(2);	mad_GPIO_Clr(mad_GPIO_LED_2);
	mad_TIMER1_WAIT_10ms(2);	mad_GPIO_Clr(mad_GPIO_LED_3);
	mad_TIMER1_WAIT_10ms(2);	mad_GPIO_Clr(mad_GPIO_LED_4);
	mad_TIMER1_WAIT_10ms(2);	mad_GPIO_Set(mad_GPIO_LED_1);
	mad_TIMER1_WAIT_10ms(2);	mad_GPIO_Set(mad_GPIO_LED_2);			//
	mad_TIMER1_WAIT_10ms(2);	mad_GPIO_Set(mad_GPIO_LED_3);
	mad_TIMER1_WAIT_10ms(2);	mad_GPIO_Set(mad_GPIO_LED_4);
	}
*/
	mad_TIMER1_WAIT_10ms(20);	mad_GPIO_Clr(mad_GPIO_LED_1);			//
	mad_TIMER1_WAIT_10ms(20);	mad_GPIO_Clr(mad_GPIO_LED_2);
	mad_TIMER1_WAIT_10ms(20);	mad_GPIO_Clr(mad_GPIO_LED_3);
	mad_TIMER1_WAIT_10ms(20);	mad_GPIO_Clr(mad_GPIO_LED_4);



	mad_TIMER1_WAIT_10ms(20);	mad_GPIO_Set(mad_GPIO_LED_1);
	mad_TIMER1_WAIT_10ms(20);	mad_GPIO_Set(mad_GPIO_LED_2);			//
	mad_TIMER1_WAIT_10ms(20);	mad_GPIO_Set(mad_GPIO_LED_3);
	mad_TIMER1_WAIT_10ms(20);	mad_GPIO_Set(mad_GPIO_LED_4);

	mad_TIMER1_WAIT_10ms(100);
	mad_GPIO_Clr(mad_GPIO_LED_1);			//
	mad_GPIO_Clr(mad_GPIO_LED_2);
//while(1){}



	//int tm1, tm2, i = 1;
/*
while(1){

		mad_GPIO_Set(mad_GPIO_OUT_1);
		mad_GPIO_Clr(mad_GPIO_OUT_1);
		mad_GPIO_Set(mad_GPIO_OUT_2);
		mad_GPIO_Clr(mad_GPIO_OUT_2);
		mad_GPIO_Set(mad_GPIO_OUT_3);
		mad_GPIO_Clr(mad_GPIO_OUT_3);
		mad_GPIO_Set(mad_GPIO_OUT_4);
		mad_GPIO_Clr(mad_GPIO_OUT_4);
		mad_GPIO_Set(mad_GPIO_OUT_5);
		mad_GPIO_Clr(mad_GPIO_OUT_5);
		mad_GPIO_Set(mad_GPIO_OUT_6);
		mad_GPIO_Clr(mad_GPIO_OUT_6);

		if(mad_GPIO_Get(mad_GPIO_CONTACT_IN) == true)
			TxChar[0] += 0x01;		//ContactIn  = 0x01
		else
			TxChar[0] -= 0x01;		//ContactIn  = 0x01

}
*/
/*
	while(1){
	//電源ON時はLEDすべて消灯
	  	mad_GPIO_Set(mad_GPIO_LED_1);
	  	mad_GPIO_Set(mad_GPIO_LED_2);
	  	mad_GPIO_Set(mad_GPIO_LED_3);
	  	mad_GPIO_Set(mad_GPIO_LED_4);
	  	mad_GPIO_Set(mad_GPIO_LED_5);
	//電源ON時は接点出力すべてOFF
	  	mad_GPIO_Clr(mad_GPIO_LED_1);
	  	mad_GPIO_Clr(mad_GPIO_LED_2);
	  	mad_GPIO_Clr(mad_GPIO_LED_3);
	  	mad_GPIO_Clr(mad_GPIO_LED_4);
	  	mad_GPIO_Clr(mad_GPIO_LED_5);
	}
*/

//#ifdef raspberry
//	mad_USART0_INIT(230400);		//ラズパイ向けUARTの通信速度指定
//#else if armadillo
	mad_USART0_INIT(921600);		//Armadillo向けUARTの通信速度指定
//#endif
	MeasureStatus = READY;			//


	mad_ADXL355_Init();		//ADXL355 初期化

	mad_TIMER1_OverFlow = false;			//タイマ割込み用フラグのクリア
	GPS_Active_Count = GPS_Active_Count_Default;

	GPS_1PPS_Enabled = false;

	int	tmp1, tmp2;
	tmp1 = (GPIO->P[mad_PortD].DOUT) & 0x07FF;				//PortDの出力状態を取り出す		Lなら点灯
	tmp2 = 0x0000;	//LED全点灯
	GPIO->P[mad_PortD].DOUT = tmp1 | tmp2;

//-------------------------------------------------------------------------------------------ここから無限ループ
	while(1){
//		mad_USART0_TxStr("2019\n");

		GPS_1PPS_Enabled = false;

 		while(mad_TIMER1_OverFlow != true ){		//1SPS時間経過するまでタイマ割込みで待つ
			if(mad_GPIO_Get(mad_GPIO_GPS_1PPS) == true)	GPS_1PPS_Enabled = true;	//1PPSがHになっていたら 1PPS検出フラグを立てる（GPSモジュールがおかしいとHになりっぱなしになる）
			CommandAnalsis();			//PCからのデータ受信と解釈、実行
		};

		mad_TIMER1_OverFlow = false;			//タイマ割込み用フラグのクリア
		SAMPLE_Pulse();							//サンプリングタイミング確認用のパルス出力

#ifdef debug
		MeasureStatus = RUN;			//Debug用、強制的に測定を実行
#endif

//測定実行フラグが立っていたら測定実行
		if(MeasureStatus != READY ){
			GetTxString();							//Status、Inc、Time、Axelデータを送信データ列に変換
			for(int i = 0; i< 44 ; i++)	mad_USART0_TxChar(TxChar[i]);	//データの送信
			if(++DataInc == 0x4000) DataInc = 0;	//14bitがオーバーフローしたらゼロクリア、16384Dataは500SPSのときに32秒
		}


#ifndef NoGPS
//GPZDAの受信完了処理
		mad_GPS_ZDA_Evaluate();
		if(mad_GPS_DATA.Error == true)mad_GPS_BufClr();	//データエラーだったら受信バッファクリアをクリア
		else if(mad_GPS_DATA.RCVD == RCVD_TIME){		//時刻受信完了ならば
			GetTtime();									//GPZDAの日時データを基にt_time値を得る
			mad_GPS_BufClr();							//受信完了したのでバッファクリア
		}

//1PPS受信処理
		if(GPS_1PPS_Enabled == true){					//1PPSがHになっていたら
			GPIO->P[mad_PortD].DOUT = GPIO->P[mad_PortD].DOUT & 0xF7FF; //GPS受信LED点灯	PD11
			GPS_Active = true;
			GPS_Active_Count = GPS_Active_Count_Default;
		}
		else{											//1PPSがLになっていたら
			GPS_Active_Count--;
			if(GPS_Active_Count == 0){
				GPS_Active_Count = 1;
				GPIO->P[mad_PortD].DOUT = GPIO->P[mad_PortD].DOUT | 0x0800; //GPS受信LED消灯
				GPS_Active = false;
			}
		}
#endif
//		CommandAnalsis();			//PCからのデータ受信と解釈、実行
	}
}
//GPS受信可否は、GPZDAだけ見ててもNG。
//1PPSは、測位できていないときには出ない。

//1PPSのH時間は、測位出来ている時に約100ms、測位できていないともうすこし短いらしい。計算量に依存？
//100SPSのときには、割込み100回に1回
//500SPSのときには、
//--------------------------------------------------------------------------------------------
void	CommandAnalsis(void)
{
	//ここを処理してる間にも、受信が継続しているかもしれない。
	//いちどバッファにコピーしてからコピー内容を判定して処理する形だと、受信中にバッファクリアした結果
	//受信完了しているか、エラーが起きてないるか、だけを教えてもらって、検査する

	//データ受信割込みは、empty、overflow、enter、indexを更新する。
	//受信は終わっているかもしれないし、終わっていないかもしれない。

	//
	//1文字受信して、それがCではなかった場合、オーバーフローフラグが立ってここに来る。
	//バッファをクリアして、受信を再開する。

	//割込みでは、受信エラーか受信完了のときに受信を停止する。
	//受信の再開は、エラー検出かコマンド実行時におこなう。

	//受信再開以降に、


	//そもそも、受信データの確認は10msecに1回しかしない。これが良くない。
	//コマンドは4char、921000bpsだと、43μ秒だ。


	uint32_t	index_tmp = mad_UART0_RX_BUF.index;
	bool		empty_tmp = mad_UART0_RX_BUF.empty;			//現状では使っていない。初期化時にtrueであるべき。
	bool		overflow_tmp = mad_UART0_RX_BUF.overflow;	//
	uint8_t		enter_tmp = mad_UART0_RX_BUF.enter;			//entrerを受信した数を保存
	unsigned int	tmp1, tmp2;


	if(empty_tmp == true){									//受信データ無しならreturn
		if((USART0->IEN & USART_IF_RXDATAV) == 0){			//受信データ無しなのに割込み有効フラグがクリアされていたらバッファクリアして受信開始
			mad_USART0_RxBufClr();
			mad_USART0_RxStart();
		}
		return;	//受信してなければ終了
	}


	if(overflow_tmp == true){	//オーバーフローなら、バッファクリアして終了
		mad_USART0_RxBufClr();
		mad_USART0_RxStart();
		return;
	}
	if(index_tmp > 4){	//受信エラーなら、バッファクリアして終了
		mad_USART0_RxBufClr();
		mad_USART0_RxStart();
		return;
	}
	if(enter_tmp == false)
		return;						//受信完了していなければreturn

	else{			//受信完了
		switch(mad_UART0_RX_BUF.data[1]){		//コマンド種類ごとに分岐
		case '0':	 //プロンプトを返す
			mad_USART0_TxPrompt();
			mad_USART0_RxStart();	//ともかく4ch受信していたら、受信を再開
			mad_USART0_RxBufClr();
			return;
		case '1':	//ID読み出し
			mad_USART0_TxStr("RQS2019");
			mad_USART0_RxStart();	//ともかく4ch受信していたら、受信を再開
			mad_USART0_RxBufClr();
			return;
		case'2':	//LED出力
																	//PD11,12,13,14がLED　5432 1xxx xxxx xxxx
			tmp1 = (GPIO->P[mad_PortD].DOUT) & 0x07FF;				//PortDの出力状態を取り出す		Lなら点灯
			tmp2 = (unsigned int)mad_UART0_RX_BUF.data[2];
			tmp2 = tmp2 << 10;
			tmp2 = tmp2 & 0xF800;	//あたらしい出力
			GPIO->P[mad_PortD].DOUT = tmp1 | tmp2;					//(mad_UART0_RX_BUF.data[2] << 11) & tmp; //PD11の状態とORしてからシフト

			mad_USART0_RxStart();	//ともかく4ch受信していたら、受信を再開
			mad_USART0_RxBufClr();
			return;
		case'3':	//接点出力
			GPIO->P[mad_PortC].DOUT = mad_UART0_RX_BUF.data[2] << 6;

			mad_USART0_RxStart();	//ともかく4ch受信していたら、受信を再開
			mad_USART0_RxBufClr();
			return;

		case '4':	//計測開始、終了、１回
			switch(mad_UART0_RX_BUF.data[2]){		//コマンド種類ごとに分岐
			case '0':	//計測終了
				MeasureStatus = READY;
				mad_USART0_RxStart();	//ともかく4ch受信していたら、受信を再開
				mad_USART0_RxBufClr();
				return;
			case '1':	//計測開始
				DataInc = 0;	//インクリメントカウンタをゼロクリア
				MeasureStatus = RUN;
				mad_ADXL355_Init();		//ADXL355 初期化
				mad_USART0_RxStart();	//ともかく4ch受信していたら、受信を再開
				mad_USART0_RxBufClr();
				return;
			case '2':	//1回計測
				MeasureStatus = ONCE;
				mad_USART0_RxBufClr();
				mad_ADXL355_Init();		//ADXL355 初期化
				mad_USART0_RxStart();	//ともかく4ch受信していたら、受信を再開
				mad_USART0_RxBufClr();
				return;
			}
			default:
				mad_USART0_RxStart();	//ともかく4ch受信していたら、受信を再開
				mad_USART0_RxBufClr();
				return;
		}
	}
}
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
void	GetTxString(void)
{
	mad_ADXL355_GetData();
//Status
	TxChar[0] = 0x80;
		if(GPS_Active == true)							TxChar[0] += 0x04;					//GPS_ACTIVE = 0x04
//else											TxChar[0] += 0x00;
		if(mad_GPIO_Get(mad_GPIO_GPS_1PPS) == true)		TxChar[0] += 0x02;		//1PPS		 = 0x02
		if(mad_GPIO_Get(mad_GPIO_CONTACT_IN) == true) 	TxChar[0] += 0x01;		//ContactIn  = 0x01

//Inc
	TxChar[1] = (DataInc >> 7) & 0x7F;			//Inc H			DataIncは32bit
	TxChar[2] = DataInc & 0x7F;				//Inc L
//Time
	TxChar[3] = (mad_SYSTEM_TIME >> 25) & 0x7F;
	TxChar[4] = (mad_SYSTEM_TIME >> 18) & 0x7F;
	TxChar[5] = (mad_SYSTEM_TIME >> 11) & 0x7F;
	TxChar[6] = (mad_SYSTEM_TIME >>  4) & 0x7F;
	TxChar[7] = (mad_SYSTEM_TIME      ) & 0x0F;


//AXXL_AXEL_DATA[CH][AXIS]に、本来の加速度値の16倍の値を格納されている。
//		ADXL		<< 4	UART
//	H	19:12		23:16	19:13
//	M	11: 4		15: 8	12: 6
//	L	 3: 0		 7:	0	 5: 0
//AXEL_CH1
	TxChar[ADXL355_CH1_X_H] = (ADXL_AXEL_DATA[CH1][AXIS_X] >> 13) & 0x7F;	//[19:12]
	TxChar[ADXL355_CH1_X_M] = (ADXL_AXEL_DATA[CH1][AXIS_X] >>  6) & 0x7F;	//[11: 4]
	TxChar[ADXL355_CH1_X_L] = (ADXL_AXEL_DATA[CH1][AXIS_X]      ) & 0x3F;	//[ 3: 0]
	TxChar[ADXL355_CH1_Y_H] = (ADXL_AXEL_DATA[CH1][AXIS_Y] >> 13) & 0x7F;
	TxChar[ADXL355_CH1_Y_M] = (ADXL_AXEL_DATA[CH1][AXIS_Y] >>  6) & 0x7F;
	TxChar[ADXL355_CH1_Y_L] = (ADXL_AXEL_DATA[CH1][AXIS_Y]      ) & 0x3F;
	TxChar[ADXL355_CH1_Z_H] = (ADXL_AXEL_DATA[CH1][AXIS_Z] >> 13) & 0x7F;
	TxChar[ADXL355_CH1_Z_M] = (ADXL_AXEL_DATA[CH1][AXIS_Z] >>  6) & 0x7F;
	TxChar[ADXL355_CH1_Z_L] = (ADXL_AXEL_DATA[CH1][AXIS_Z]      ) & 0x3F;
	if(mad_ADXL_355_ERROR_CH1 == true) TxChar[ADXL355_CH1_X_L] += 0x40;
//AXEL_CH2
	TxChar[ADXL355_CH2_X_H] = (ADXL_AXEL_DATA[CH2][AXIS_X] >> 13) & 0x7F;
	TxChar[ADXL355_CH2_X_M] = (ADXL_AXEL_DATA[CH2][AXIS_X] >>  6) & 0x7F;
	TxChar[ADXL355_CH2_X_L] = (ADXL_AXEL_DATA[CH2][AXIS_X]      ) & 0x3F;
	TxChar[ADXL355_CH2_Y_H] = (ADXL_AXEL_DATA[CH2][AXIS_Y] >> 13) & 0x7F;
	TxChar[ADXL355_CH2_Y_M] = (ADXL_AXEL_DATA[CH2][AXIS_Y] >>  6) & 0x7F;
	TxChar[ADXL355_CH2_Y_L] = (ADXL_AXEL_DATA[CH2][AXIS_Y]      ) & 0x3F;
	TxChar[ADXL355_CH2_Z_H] = (ADXL_AXEL_DATA[CH2][AXIS_Z] >> 13) & 0x7F;
	TxChar[ADXL355_CH2_Z_M] = (ADXL_AXEL_DATA[CH2][AXIS_Z] >>  6) & 0x7F;
	TxChar[ADXL355_CH2_Z_L] = (ADXL_AXEL_DATA[CH2][AXIS_Z]      ) & 0x3F;
	if(mad_ADXL_355_ERROR_CH2 == true) TxChar[ADXL355_CH2_X_L] += 0x40;
//AXEL_CH3
	TxChar[ADXL355_CH3_X_H] = (ADXL_AXEL_DATA[CH3][AXIS_X] >> 13) & 0x7F;
	TxChar[ADXL355_CH3_X_M] = (ADXL_AXEL_DATA[CH3][AXIS_X] >>  6) & 0x7F;
	TxChar[ADXL355_CH3_X_L] = (ADXL_AXEL_DATA[CH3][AXIS_X]      ) & 0x3F;
	TxChar[ADXL355_CH3_Y_H] = (ADXL_AXEL_DATA[CH3][AXIS_Y] >> 13) & 0x7F;
	TxChar[ADXL355_CH3_Y_M] = (ADXL_AXEL_DATA[CH3][AXIS_Y] >>  6) & 0x7F;
	TxChar[ADXL355_CH3_Y_L] = (ADXL_AXEL_DATA[CH3][AXIS_Y]      ) & 0x3F;
	TxChar[ADXL355_CH3_Z_H] = (ADXL_AXEL_DATA[CH3][AXIS_Z] >> 13) & 0x7F;
	TxChar[ADXL355_CH3_Z_M] = (ADXL_AXEL_DATA[CH3][AXIS_Z] >>  6) & 0x7F;
	TxChar[ADXL355_CH3_Z_L] = (ADXL_AXEL_DATA[CH3][AXIS_Z]      ) & 0x3F;
	if(mad_ADXL_355_ERROR_CH3 == true) TxChar[ADXL355_CH3_X_L] += 0x40;
//AXEL_CH4
	TxChar[ADXL355_CH4_X_H] = (ADXL_AXEL_DATA[CH4][AXIS_X] >> 13) & 0x7F;
	TxChar[ADXL355_CH4_X_M] = (ADXL_AXEL_DATA[CH4][AXIS_X] >>  6) & 0x7F;
	TxChar[ADXL355_CH4_X_L] = (ADXL_AXEL_DATA[CH4][AXIS_X]      ) & 0x3F;
	TxChar[ADXL355_CH4_Y_H] = (ADXL_AXEL_DATA[CH4][AXIS_Y] >> 13) & 0x7F;
	TxChar[ADXL355_CH4_Y_M] = (ADXL_AXEL_DATA[CH4][AXIS_Y] >>  6) & 0x7F;
	TxChar[ADXL355_CH4_Y_L] = (ADXL_AXEL_DATA[CH4][AXIS_Y]      ) & 0x3F;
	TxChar[ADXL355_CH4_Z_H] = (ADXL_AXEL_DATA[CH4][AXIS_Z] >> 13) & 0x7F;
	TxChar[ADXL355_CH4_Z_M] = (ADXL_AXEL_DATA[CH4][AXIS_Z] >>  6) & 0x7F;
	TxChar[ADXL355_CH4_Z_L] = (ADXL_AXEL_DATA[CH4][AXIS_Z]      ) & 0x3F;
	if(mad_ADXL_355_ERROR_CH4 == true) TxChar[ADXL355_CH4_X_L] += 0x40;



	return;
}
//--------------------------------------------------------------------------------------------
void	GetTtime(void)
{
	char	HOUR[8],MIN[8],SEC[8];
	HOUR[0] = mad_GPS_DATA.TIME[0];
	HOUR[1] = mad_GPS_DATA.TIME[1];
	HOUR[2] = 0x00;
	MIN[0]  = mad_GPS_DATA.TIME[2];
	MIN[1]  = mad_GPS_DATA.TIME[3];
	MIN[2]  = 0x00;
	SEC[0]  = mad_GPS_DATA.TIME[4];
	SEC[1]  = mad_GPS_DATA.TIME[5];
	SEC[2]  = 0x00;

	struct	tm*	tm_time;
	tm_time = localtime(&mad_SYSTEM_TIME);				//いちどt_time型にしてから
	tm_time->tm_year 	 = atoi(mad_GPS_DATA.YEAR)-1900;
	tm_time->tm_mon 	 = atoi(mad_GPS_DATA.MONTH) - 1;
	tm_time->tm_mday 	 = atoi(mad_GPS_DATA.DAY);
	tm_time->tm_hour = atoi(HOUR);	//時の単位を、09:00＋UID（0～4）の値にセットする。
	tm_time->tm_min  = atoi(MIN);	//分の単位をゼロにする
	tm_time->tm_sec  = atoi(SEC);	//							//秒の単位をゼロにする

	mad_SYSTEM_TIME = mktime(tm_time);
}
//--------------------------------------------------------------------------------------------

