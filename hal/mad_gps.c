//--------------------------------------------------------------------------------------------
//Copyright 2019 Mathematical Assist Design Laboratories, Inc.
//All Rights Reserved.
//--------------------------------------------------------------------------------------------
#include "mad_gps.h"
#include "mad_usart.h"


uint8_t mad_GPS_DATA_MEM[16];		//メモリ保存用のバッファ
GPS_INT		mad_GPS_INT;
GPS_DATA	mad_GPS_DATA, mad_GPS_DATA_backup;
uint32_t		mad_GPS_TX_DATA[16];


//UBX-CFG-MSG
static char GPGLL_OFF[16] 	= {	0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x01,	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2A};
static char GPGSA_OFF[16] 	= {	0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x02,	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x31};
static char GPGSV_OFF[16] 	= {	0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x03,	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x38};
static char GPRMC_OFF[16] 	= {	0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x04,	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x3F};
static char GPVTG_OFF[16] 	= {	0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x05,	0x00, 0x00,	0x00, 0x00, 0x00, 0x00, 0x04, 0x46};
//static char GPZDA_OFF[16]	 = {	0xB5, 0x62, 0x06, 0x01, 0x06, 0x00, 0xF0, 0x08, 0x00, 0x00, 0x00, 0x00, 0x05, 0x3D};


static char GPGGA_ON[16]  	= {	0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x00,	0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x28};
static char GPGGA_OFF[16] 	= {	0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x00,	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x23};
static char GPZDA_ON[16]  	= {	0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x08,	0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x08, 0x60};
static char GPZDA_OFF[16] 	= {	0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x08,	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x5B};

//UBX-CFG-RPT
static char GPS_9600bps[32] = {	0xB5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x01, 0x00,
								0x00, 0x00, 0xD0, 0x08,	0x00, 0x00, 0x80, 0x25,
								0x00, 0x00, 0x07, 0x00, 0x03, 0x00, 0x00, 0x00,
								0x00, 0x00, 0xA2, 0xB5};
static char GPS_460kbps[32] = {	0xB5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x01, 0x00,
								0x00, 0x00, 0xD0, 0x08,	0x00, 0x00, 0x00, 0x08,
								0x07, 0x00, 0x07, 0x00, 0x03, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x0C, 0xBC};
static char GPS_920kbps[32] = {	0xB5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x01, 0x00,
								0x00, 0x00, 0xD0, 0x08,	0x00, 0x00, 0x00, 0x10,
								0x0E, 0x00, 0x07, 0x00, 0x03, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x1B, 0x5A};

//--------------------------------------------------------------------------------------------
void	mad_GPS_INIT(void)
{
  	mad_GPS_ON();	//ONしてから初期化するまでの間にWAITが必要
	mad_USART1_INIT(9600);			//ZOE-M8G
	mad_USART1_RxStop(); // ★ Init 直後は受信停止 ★
	mad_TIMER1_WAIT_10ms(100);

	int	i;
    // --- ▼▼▼ printf デバッグ (ラベル修正) ▼▼▼ ---
    printf("GPS_INIT: Checkpoint 1 - Before GPGSA loop\n"); fflush(stdout);
	for(i = 0; i<16 ; i++)	mad_USART1_TxChar(GPGSA_OFF[i]);
    printf("GPS_INIT: Checkpoint 2 - After GPGSA loop\n"); fflush(stdout);

    printf("GPS_INIT: Checkpoint 3 - Before GPGSV loop\n"); fflush(stdout);
	for(i = 0; i<16 ; i++)	mad_USART1_TxChar(GPGSV_OFF[i]);
    printf("GPS_INIT: Checkpoint 4 - After GPGSV loop\n"); fflush(stdout);

    printf("GPS_INIT: Checkpoint 5 - Before GPGLL loop\n"); fflush(stdout);
	for(i = 0; i<16 ; i++)	mad_USART1_TxChar(GPGLL_OFF[i]);
    printf("GPS_INIT: Checkpoint 6 - After GPGLL loop\n"); fflush(stdout);

    printf("GPS_INIT: Checkpoint 7 - Before GPRMC loop\n"); fflush(stdout);	
	for(i = 0; i<16 ; i++)	mad_USART1_TxChar(GPRMC_OFF[i]);
    printf("GPS_INIT: Checkpoint 8 - After GPRMC loop\n"); fflush(stdout);	

    printf("GPS_INIT: Checkpoint 9 - Before GPVTG loop\n"); fflush(stdout);	
	for(i = 0; i<16 ; i++)	mad_USART1_TxChar(GPVTG_OFF[i]);
    printf("GPS_INIT: Checkpoint 10 - After GPVTG loop\n"); fflush(stdout);

	printf("GPS_INIT: Checkpoint 11 - Before GPGGA_OFF loop\n"); fflush(stdout);
	for(i = 0; i<16 ; i++)	mad_USART1_TxChar(GPGGA_OFF[i]);
    printf("GPS_INIT: Checkpoint 12 - After GPGGA_OFF loop\n"); fflush(stdout);

    printf("GPS_INIT: Checkpoint 13 - Before GPZDA_ON loop\n"); fflush(stdout);
	for(i = 0; i<16 ; i++)	mad_USART1_TxChar(GPZDA_ON[i]);
    printf("GPS_INIT: Checkpoint 14 - After GPZDA_ON loop\n"); fflush(stdout);
    // --- ▲▲▲ printf デバッグ (ラベル修正) ▲▲▲ ---
}

//--------------------------------------------------------------------------------------------
bool	mad_GPS_GetZDA(void)	//GPZDA�̍s�������M����̐��𐮂���
{
	mad_GPS_BufClr();
	mad_USART1_RxStart();
	mad_GPS_BufClr();

//		while(mad_UART1_RX_BUF.enter = 1){};	//enter����M����܂ő҂��Ȃ�
		mad_GPS_ZDA_Evaluate();
		if(mad_GPS_DATA.Error == true){
			mad_GPS_BufClr();		//�f�[�^�G���[�̂Ƃ��́A�o�b�t�@���N���A
			return false;}
		else if(mad_GPS_DATA.RCVD == RCVD_TIME){	//�ړI�̃f�[�^������ꂽ��I��
			return true;
		}
//		mad_GPS_BufClr();					//�G���[�ł͂Ȃ����ړI�f�[�^�������Ă��Ȃ�������o�b�t�@�N���A���Ă��Ȃ���

	mad_GPS_DATA.Error = true;
	return false;
}
//--------------------------------------------------------------------------------------------
bool	mad_GPS_GetData(uint16_t count, GPS_RCVD RCVD)
{
	//GPRMC�̍s�������M����̐��𐮂���B
	//ZOE-M8G��NMEA�́AGNRMC->GNGGA�̏��B
	//GNRMC+GNGGA�̃��b�Z�[�W���Ԃ́A9600bps�̂Ƃ���154msec�B

		//��M�o�b�t�@���N���A���A�s���܂��̓I�[�o�[�t���[�܂��̓^�C���A�E�g�܂Ŏ�M����B
		//Enter�[���܂��͂ЂƂȂ�΁A��M���s
		//Enter���ӂ��ȏ��M���Ă�����\����͂����s
		//�G���[�����o������150msec�҂��Ă�����Ȃ���
		//�ړI�̃f�[�^����M�ł��Ă�����I���A�ł��Ă��Ȃ���������Ȃ���
	mad_GPS_BufClr();
	mad_USART1_RxStart();
	mad_GPS_BufClr();
	for( ; count != 0; count--){
		while(mad_UART1_RX_BUF.enter <= 1){};	//enter���ӂ���M����܂ő҂�
		mad_GPIO_Clr(mad_GPIO_LED_5);
		mad_GPS_Evaluate();
		mad_GPIO_Set(mad_GPIO_LED_5);

		if(mad_GPS_DATA.Error == true){
			mad_GPS_BufClr();
			mad_TIMER1_WAIT_10ms(15);		//�f�[�^�G���[�̂Ƃ��́A150msec�҂B
			continue;}

		else if(mad_GPS_DATA.RCVD >= RCVD){	//�ړI�̃f�[�^������ꂽ��I��

			return true;
		}
		mad_GPS_BufClr();					//�G���[�ł͂Ȃ����ړI�f�[�^�������Ă��Ȃ�������o�b�t�@�N���A���Ă��Ȃ���
	}
	mad_GPS_DATA.Error = true;

	return false;
}
/*
$GNRMC,024932.00,V,,,,,,,301218,,,N*64
$GNGGA,024932.00,,,,,0,04,44.94,,,,,,*7F
"} index=82 empty=false overflow=false enter=2 ('\002') "

//������M�ł��ĂȂ�
data={"$GNRMC,,V,,,,,,,,,,N*4D
$GNGGA,,,,,,0,00,99.99,,,,,,*56
"} index=58 empty=false overflow=false enter=2 ('\002')


//���r�ώ@�ّO�@�QD�@�@���n�Ŏ擾
"$GNRMC,041332.00,A,3619.30661,N,13617.65887,E,0.171,,301218,,,A*6E
$GNGGA,041332.00,3619.30661,N,13617.65887,E,1,04,2.57,17.5,M,34.3,M,,*7F
"
"$GNRMC,050503.00,A,3619.30599,N,13617.65663,E,0.087,,301218,,,A*62
$GNGGA,050503.00,3619.30599,N,13617.65663,E,1,04,2.20,11.7,M,34.3,M,,*7F
"

//���r�ώ@�ّO�@3D�@���n�Ŏ擾

"$GNRMC,050553.00,A,3619.30735,N,13617.65911,E,0.050,,301218,,,A*63
$GNGGA,050553.00,3619.30735,N,13617.65911,E,1,05,1.65,6.0,M,34.3,M,,*46
"



//���r�@�Ί݁@�QD
"$GNRMC,064530.00,A,3619.32999,N,13617.23222,E,0.056,,301218,,,A*64
$GNGGA,064530.00,3619.32999,N,13617.23222,E,1,04,2.28,23.7,M,34.3,M,,*7C
"

"$GNRMC,065055.00,A,3619.34408,N,13617.23067,E,0.080,,301218,,,A*68
$GNGGA,065055.00,3619.34408,N,13617.23067,E,1,05,3.07,40.2,M,34.3,M,,*76
"


//���r�ώ@�ّO�@3D�@���n�Ŏ擾�A�ł�GPGGA�̌㔼�A�q�����̂����肪���������B
"$GNRMC,041447.00,A,3619.30260,N,13617.65544,E,0.017,,301218,,,A*6D
$GNGGA,041447.00,3619.30260,N,13617.65544,E,1,22.1,M,,*70
"


//���r�ώ@�ّO�Ŏ擾�@�QD���ʂł���Ƃ��Č딻��
"$GNRMC,045003.00,A,3619.30613,N,13617.653989,,,,,ANGGA,045003.00,3619.30613,N,13617.65393,E,1,04,2.96,21.7,M,34.3,M,,*7B
$GNRMC,045004.00,A,3619.30613,N,13617.65391,E,0.174,,301218,,,A*60
$GN
"


�������������Ă��Ȃ��̂ɂQD fix�ɂȂ��Ă��܂� bug
$GNGGA,025148.00,,,,,0,04,51.87,,,,,,*7D
$GNRMC,025149.00,V,,,,,,,301218,,,N*61
"} index=82 empty=false overflow=false enter=2 ('\002') 

*/
//--------------------------------------------------------------------------------------------
char	*mad_GPS_StrToken(char *s1, char s2)
{
    static char *str = 0;
    if (s1) {
        str = s1;
    } else {
        s1 = str;
    }
    if (!s1) { return(0); }
    while (1) {
        if (!*str) {
            str = 0;
            return(s1);
        }
        if (*str == s2) {
            *str++ = 0;
            return(s1);
        }
        str++;
    }
}
//--------------------------------------------------------------------------------------------
void	mad_GPS_ZDA_Evaluate(void)
{//GxZDA����M���āAGPS_DATA�^�i�������Aday,month,year,time�̂�)��Ԃ��B
//$GNZDA,064349.00,26,03,2019,00,00*79
	char *tp;

	strcpy(mad_GPS_DATA.TIME,"000000");	//[000000]
	strcpy(mad_GPS_DATA.DAY,"1");		//[1-31]
	strcpy(mad_GPS_DATA.MONTH,"1");		//[1-12]		//NMEA�ł�[1-12]�Ȃ̂ŁA�����ł�[0-11]�ł͂Ȃ�[1-12]
	strcpy(mad_GPS_DATA.YEAR,"1970");	//[1900-]

	if(mad_UART1_RX_BUF.empty == true)return ;									//��M�o�b�t�@���������
	if(mad_UART1_RX_BUF.data[mad_UART1_RX_BUF.index - 1] != '\n')return ;		//�s�������s����Ȃ�
	if(mad_UART1_RX_BUF.enter == 0)return ;										//�s���܂Ŏ�M���Ă��Ȃ�

	if(mad_UART1_RX_BUF.enter != 1){mad_GPS_DATA.Error = true; return ;} 		//�s���𕡐���M����
	if(mad_UART1_RX_BUF.overflow == true){mad_GPS_DATA.Error = true; ; return ;}//�I�[�o�[�t���[����
//	if(mad_UART1_RX_BUF.index < 38)return ;										//��M������37������菭�Ȃ�


	mad_GPS_DATA.RCVD = RCVD_NONE;
	mad_GPS_DATA.Error = false;


	tp = mad_GPS_StrToken(mad_UART1_RX_BUF.data, ',');		//$GNZDA or error�@�H
	if(strcmp(tp,"$GNZDA") == true){mad_GPS_DATA.Error = true; mad_GPS_DATA.RCVD = RCVD_NONE;return;} 	//�s�̐擪��$GNZDA�ȊO��������error
	tp = mad_GPS_StrToken(NULL,'.');	strcpy(mad_GPS_DATA.TIME,tp);
	tp = mad_GPS_StrToken(NULL,',');	//
	tp = mad_GPS_StrToken(NULL,',');	strcpy(mad_GPS_DATA.DAY,tp);
	tp = mad_GPS_StrToken(NULL,',');	strcpy(mad_GPS_DATA.MONTH,tp);
	tp = mad_GPS_StrToken(NULL,',');	strcpy(mad_GPS_DATA.YEAR,tp);

	if(mad_GPS_DATA.TIME[0] == 0x00)	{mad_GPS_DATA.RCVD = RCVD_NONE;return;}			//�������[���Ȃ�ANONE
	else if(mad_GPS_DATA.Error == true){mad_GPS_DATA.RCVD = RCVD_NONE;return;}			//�����f�[�^��������ANONE
	else                                {mad_GPS_DATA.RCVD = RCVD_TIME  ;return;}		//�����łȂ���΁A�����擾����

}
//--------------------------------------------------------------------------------------------
void	mad_GPS_Evaluate(void)
{//NMEA���b�Z�[�W�̃G���[����ƃf�[�^����
	char *tp;
	char rxdata[512];
//	sprintf(rxdata, "$GNGGA,025148.00,,,,,0,04,51.87,,,,,,*7D\n$GNRMC,025149.00,V,,,,,,,301218,,,N*61");	//�������������Ă��Ȃ��̂ɂQD fix�ɂȂ��Ă��܂� bug
//	sprintf(rxdata, "$GNRMC,041447.00,A,3619.30260,N,13617.65544,E,0.017,,301218,,,A*6D\n$GNGGA,041447.00,3619.30260,N,13617.65544,E,1,22.1,M,,*70");//�q�����̂����肪���������B
//	sprintf(rxdata, "$GNGGA,065055.00,3619.34408,N,13617.23067,E,1,05,3.07,40.2,M,34.3,M,,*76\n$GNRMC,065055.00,A,3619.34408,N,13617.23067,E,0.080,,301218,,,A*68");
//	sprintf(rxdata, "$GNRMC,050553.00,A,3619.30735,N,13617.65911,E,0.050,,301218,,,A*63\n$GNGGA,050553.00,3619.30735,N,13617.65911,E,1,05,1.65,6.0,M,34.3,M,,*46");
	if(mad_UART1_RX_BUF.empty == true)return;										//��M�o�b�t�@���������
	if(mad_UART1_RX_BUF.overflow == true)	{mad_GPS_DATA.Error = true; return;} 	//�I�[�o�[�t���[����
	if(mad_UART1_RX_BUF.enter != 2)return;	//���s���ӂ���M���Ă��Ȃ�

	mad_GPS_DATA.RCVD = RCVD_NONE;
	mad_GPS_DATA.Error = false;

	strcpy(rxdata,mad_UART1_RX_BUF.data);
	if(strlen(rxdata) < 60){mad_GPS_DATA.RCVD = RCVD_NONE;return;}

	tp = mad_GPS_StrToken(rxdata, ',');	//$GNGGA or $GNRMC or error
	if(strcmp(tp,"$GNRMC") == false){		//GNRMC�撅�̏ꍇ
		tp = mad_GPS_StrToken(NULL,',');	strcpy(mad_GPS_DATA.TIME,tp);
		tp = mad_GPS_StrToken(NULL,',');	if(strcmp(tp,"A"))mad_GPS_DATA.Error = true; // A:�L�� or V:����
		tp = mad_GPS_StrToken(NULL,',');	//�ܓx
		tp = mad_GPS_StrToken(NULL,',');	//�@N or S
		tp = mad_GPS_StrToken(NULL,',');	//�o�x
		tp = mad_GPS_StrToken(NULL,',');	//E or W
		tp = mad_GPS_StrToken(NULL,',');	//���x
		tp = mad_GPS_StrToken(NULL,',');	//�i�s����
		tp = mad_GPS_StrToken(NULL,',');	strcpy(mad_GPS_DATA.DATE,tp);
		tp = mad_GPS_StrToken(NULL,'$');	//���C�΍��ʁA���C�΍������A�`�F�b�N�T���͕]�����Ȃ�

		tp = mad_GPS_StrToken(NULL,',');	//if(strcmp(tp,"GNGGA") != false){tp = strtok(NULL,",");}
		tp = mad_GPS_StrToken(NULL,',');//	strcpy(mad_GPS_DATA.TIME,tp);
		tp = mad_GPS_StrToken(NULL,',');	strcpy(mad_GPS_DATA.LAT,tp);
		tp = mad_GPS_StrToken(NULL,',');	if(strcmp(tp,"N")) mad_GPS_DATA.NS = true;else mad_GPS_DATA.NS = false;
		tp = mad_GPS_StrToken(NULL,',');	strcpy(mad_GPS_DATA.LON,tp);
		tp = mad_GPS_StrToken(NULL,',');	if(strcmp(tp,"E")) mad_GPS_DATA.EW = true;else mad_GPS_DATA.EW = false;
		tp = mad_GPS_StrToken(NULL,',');	if(strcmp(tp,"0"))mad_GPS_DATA.Error = false;else mad_GPS_DATA.Error = true; //����:0�A�P�Ƒ���:�P 2�ɂȂ邱�Ƃ�����
		tp = mad_GPS_StrToken(NULL,',');	strcpy(mad_GPS_DATA.SAT,tp);if(atoi(tp) == 0)mad_GPS_DATA.Error = true;
		tp = mad_GPS_StrToken(NULL,',');	strcpy(mad_GPS_DATA.DOP,tp);if(atof(tp) == 0)mad_GPS_DATA.Error = true;
		tp = mad_GPS_StrToken(NULL,',');	strcpy(mad_GPS_DATA.ALT,tp);
	}
	else if	(strcmp(tp,"$GNGGA") == false){//GNGGA�撅�̏ꍇ
		tp = mad_GPS_StrToken(NULL,',');	strcpy(mad_GPS_DATA.TIME,tp);
		tp = mad_GPS_StrToken(NULL,',');	strcpy(mad_GPS_DATA.LAT,tp);
		tp = mad_GPS_StrToken(NULL,',');	if(strcmp(tp,"N")) mad_GPS_DATA.NS = true;else mad_GPS_DATA.NS = false;
		tp = mad_GPS_StrToken(NULL,',');	strcpy(mad_GPS_DATA.LON,tp);
		tp = mad_GPS_StrToken(NULL,',');	if(strcmp(tp,"E")) mad_GPS_DATA.EW = true;else mad_GPS_DATA.EW = false;
		tp = mad_GPS_StrToken(NULL,',');	if(strcmp(tp,"0"))mad_GPS_DATA.Error = false;else mad_GPS_DATA.Error = true; //����:0�A�P�Ƒ���:�P 2�ɂȂ邱�Ƃ�����
		tp = mad_GPS_StrToken(NULL,',');	strcpy(mad_GPS_DATA.SAT,tp);if(atoi(tp) == 0)mad_GPS_DATA.Error = true;
		tp = mad_GPS_StrToken(NULL,',');	strcpy(mad_GPS_DATA.DOP,tp);if(atof(tp) == 0)mad_GPS_DATA.Error = true;
		tp = mad_GPS_StrToken(NULL,',');	strcpy(mad_GPS_DATA.ALT,tp);
		tp = mad_GPS_StrToken(NULL,'$');	//�W�I�C�h���ƃ`�F�b�N�T���͕]������SKIP
		tp = mad_GPS_StrToken(NULL,',');	//if(strcmp(tp,"GNRMC") != false){tp = strtok(NULL,",");}
		tp = mad_GPS_StrToken(NULL,',');	//����
		tp = mad_GPS_StrToken(NULL,',');	if(strcmp(tp,"A"))mad_GPS_DATA.Error = true; // A:�L�� or V:����
		tp = mad_GPS_StrToken(NULL,',');	//�ܓx
		tp = mad_GPS_StrToken(NULL,',');	//�@N or S
		tp = mad_GPS_StrToken(NULL,',');	//�o�x
		tp = mad_GPS_StrToken(NULL,',');	//E or W
		tp = mad_GPS_StrToken(NULL,',');	//���x
		tp = mad_GPS_StrToken(NULL,',');	//�i�s����
		tp = mad_GPS_StrToken(NULL,',');	strcpy(mad_GPS_DATA.DATE,tp);
//		tp = mad_GPS_StrToken(NULL,'$');	//���C�΍��ʁA���C�΍������A�`�F�b�N�T���͕]�����Ȃ�
	}
	else {mad_GPS_DATA.Error = true; mad_GPS_DATA.RCVD = RCVD_NONE;return;} 	//�s�̐擪��$GNGGA�܂��́�GNRMC�ȊO��������error


	if(mad_GPS_DATA.TIME[0] == 0x00)	{mad_GPS_DATA.RCVD = RCVD_NONE;return;}			//�������[���Ȃ�ANONE
	else if(mad_GPS_DATA.LAT[0] == 0x00){mad_GPS_DATA.RCVD = RCVD_TIME;mad_GPS_DATA.Error = false;return;}	//LAT���[���Ȃ�A�G���[��false�ɂ��āATIME
	else if(mad_GPS_DATA.Error == true){mad_GPS_DATA.RCVD = RCVD_NONE;return;}			//�����f�[�^��������ANONE
	else if(atoi(mad_GPS_DATA.SAT) <= 4)		{mad_GPS_DATA.RCVD = RCVD_2D  ;return;}	//�g�p�q������5�����Ȃ�A�QD����
	else                                {mad_GPS_DATA.RCVD = RCVD_3D  ;return;}			//�g�p�q������5�ȏ�Ȃ�A�RD����

// �����݌v�������@���c�������@�������W
//	$GNRMC,040227.00,A,3621.28129,N,13922.35286,E,0.053,,041218,,,A*62
//	$GNGGA,040227.00,3621.28129,N,13922.35286,E,1,05,1.35,76.2,M,38.6,M,,*7A
}
//--------------------------------------------------------------------------------------------
void	mad_GPS_Data2Int(void)
{
	char *error;
	mad_GPS_INT.DATE = 	atoi(mad_GPS_DATA.DATE);	//ddmmyy
	mad_GPS_INT.TIME = 	atoi(mad_GPS_DATA.TIME);	//hhmmss
	mad_GPS_INT.LAT = (uint32_t)(strtod(mad_GPS_DATA.LAT, &error)*1000);	// ddmmmmm
	mad_GPS_INT.LON = (uint32_t)(strtod(mad_GPS_DATA.LON, &error)*1000);	//dddmmmmm
	mad_GPS_INT.SAT = (uint8_t)(strtod(mad_GPS_DATA.SAT, &error));			//ss
	mad_GPS_INT.DOP = (uint8_t)(strtod(mad_GPS_DATA.DOP, &error)*10);		//ss
	mad_GPS_INT.ALT = (int32_t)strtod(mad_GPS_DATA.ALT, &error);			//hhhh
	mad_GPS_INT.NS = mad_GPS_DATA.NS;
	mad_GPS_INT.EW = mad_GPS_DATA.EW;

	mad_GPS_INT.LAT_DD = mad_GPS_INT.LAT / 100000;
	mad_GPS_INT.LAT_MM = mad_GPS_INT.LAT - (mad_GPS_INT.LON_DD*100000);
	mad_GPS_INT.LON_DD = mad_GPS_INT.LON / 100000;
	mad_GPS_INT.LON_MM = mad_GPS_INT.LON - (mad_GPS_INT.LON_DD*100000);

	mad_GPS_INT.HOUR = mad_GPS_INT.TIME / 10000;
	mad_GPS_INT.MIN = (mad_GPS_INT.TIME / 100) - (mad_GPS_INT.HOUR * 100);
	mad_GPS_INT.SEC = mad_GPS_INT.TIME - (mad_GPS_INT.MIN *100) - (mad_GPS_INT.HOUR * 10000);

	mad_GPS_INT.DAY = mad_GPS_INT.DATE / 10000;
	mad_GPS_INT.MONTH = (mad_GPS_INT.DATE / 100) - (mad_GPS_INT.DAY * 100);

/*
	char		TIME[10];	//����		hhmmss.ss
	char		LAT[11];	//�ܓx		ddmm.mmmmm
	bool		NS;			//�k�܁E���	true = �k�܁Afalse=���
	char		LON[12];	//�o�x		dddmm.mmmmm
	bool		EW;			//���o�E���o	true = ���o�Afalse=���o
	char		SAT[5];		//�g�p�q����	dd
	char		DOP[5];		//HDOP�@�������x�ቺ��
	char		ALT[10];						//���x
	//GNRMC
	char		DATE[10];		//���t	ddmmyy�iUTC)
	bool		GGA_Recvd;
	bool		RMC_Recvd;
	bool		Error;
*/

	return;
}
//--------------------------------------------------------------------------------------------
void	mad_GPS_Data2Bin_GPSTX()
{
	for(int i = 0; i < 16;i++){	mad_GPS_TX_DATA[i] = 0;};	//�܂��̓o�b�t�@���N���A
	mad_GPS_Data2Int();	//���x�̕����𔻒肷�邽�߂ɁAint�ɂ��Ă���

	if(mad_GPS_DATA.Error == true)	mad_GPS_TX_DATA[0] += 0b1100000000;
	if(mad_GPS_DATA.NS == true)		mad_GPS_TX_DATA[0] += 0b0010000000;
	if(mad_GPS_DATA.EW == true)		mad_GPS_TX_DATA[0] += 0b0001000000;
	if((mad_GPS_DATA.LAT[(strlen(mad_GPS_DATA.LAT) - 9)] & 0x01) == 0x01)mad_GPS_TX_DATA[0] += 0b0000100000;
	if((mad_GPS_DATA.LON[(strlen(mad_GPS_DATA.LON) - 9)] & 0x01) == 0x01)mad_GPS_TX_DATA[0] += 0b0000010000;
	if(mad_GPS_INT.ALT < 0)			mad_GPS_TX_DATA[0] += 0b0000001000;
	if(mad_GPS_INT.SAT >= 5)		mad_GPS_TX_DATA[0] += 0b0000000100;
	if(mad_GPS_INT.DOP <= 3*10)		mad_GPS_TX_DATA[0] += 0b0000000010;
/*
	double	LAT_DD, LAT_MM, LON_DD, LON_MM;
	LAT_DD = floor((double)mad_GPS_INT.LAT / 100000);
	LAT_MM = (double)mad_GPS_INT.LAT  - (LAT_DD * 100000);
	mad_GPS_INT.LAT = (int)LAT_MM;
	LON_DD = floor(mad_GPS_INT.LON / 100000);
	LON_MM = (double)mad_GPS_INT.LON  - (LON_DD * 100000);
	mad_GPS_INT.LON = (int)LON_MM;
*/
	int lat_dd = mad_GPS_INT.LAT / 100000;
	mad_GPS_INT.LAT =  mad_GPS_INT.LAT - (lat_dd*100000);
	int lon_dd = mad_GPS_INT.LON / 100000;
	mad_GPS_INT.LON =  mad_GPS_INT.LON - (lon_dd*100000);

/*
	mad_GPS_DATA.LAT[(sizeof(mad_GPS_DATA.LAT) - 10)] = 0x30;	//�ܓx�ƌo�x�̓x�̒P�ʂ��[���N���A
	mad_GPS_DATA.LAT[(sizeof(mad_GPS_DATA.LAT) - 11)] = 0x30;
	mad_GPS_DATA.LON[(sizeof(mad_GPS_DATA.LON) - 10)] = 0x30;
	mad_GPS_DATA.LON[(sizeof(mad_GPS_DATA.LON) - 11)] = 0x30;
	mad_GPS_DATA.LON[(sizeof(mad_GPS_DATA.LON) - 12)] = 0x30;

	mad_GPS_Data2Int();	//�ܓx�ƌo�x��int�ɂ��Ă���
*/

	int32_t	tmp;
	mad_GPS_TX_DATA[1] = mad_GPS_INT.LAT >> 6;					//�ܓx�̏�ʌ��ۑ�
	mad_GPS_TX_DATA[2] = (mad_GPS_INT.LAT & 0b0111111) << 4;	//�ܓx�̉��ʌ��ۑ�
	mad_GPS_TX_DATA[2] += ((mad_GPS_INT.LON >> 12) & 0b01111);	//�o�x�̏�ʌ��ۑ�
	mad_GPS_TX_DATA[3] = ((mad_GPS_INT.LON >> 2) & 0b01111111111);	//�o�x�̒��ʌ��ۑ�
	mad_GPS_TX_DATA[4] = ((mad_GPS_INT.LON << 8) & 0b01100000000);	//�o�x�̉��ʌ��ۑ�

	tmp = mad_GPS_INT.ALT;											//���x�̕��������
	if(tmp < 0)tmp = tmp * -1;
	mad_GPS_TX_DATA[4] += ((tmp >> 6) & 0b011111111);				//���x�̏�ʌ��ۑ�
	mad_GPS_TX_DATA[5] = ((tmp << 4) & 0b01111110000);				//���x�̉��ʌ��ۑ�


	return;
//	$GNRMC,040227.00,A,3621.28129,N,13922.35286,E,0.053,,041218,,,A*62
//	$GNGGA,040227.00,3621.28129,N,13922.35286,E,1,05,1.35,76.2,M,38.6,M,,*7A
}
//--------------------------------------------------------------------------------------------
void	mad_GPS_Int2Mem(void)
{
	char tmp[16];
	for(int i = 0; i < 16;i++){	mad_GPS_DATA_MEM[i] = 0;};					//�܂��̓o�b�t�@���N���A
	mad_GPS_Data2Int();														//���x�̕����𔻒肷�邽�߂ɁA�E�����߂ɁAint�ɂ��Ă���

	if(mad_GPS_INT.ALT < 0)			mad_GPS_DATA_MEM[1] = 0b10000000;		//���x��-�������Ƃ��̏���
	//SAT
	mad_GPS_DATA_MEM[0]  = 0;												//�����ł́A�Ƃ肠�����Z���ɂ��Ă����A���Ƃ�Status������
	if(mad_GPS_INT.SAT >= 16)mad_GPS_DATA_MEM[1] += 0b01111000;  			//SAT
	else mad_GPS_DATA_MEM[1]  += mad_GPS_INT.SAT << 3;
	//NSEW
	if(mad_GPS_DATA.NS == true)		mad_GPS_DATA_MEM[1] += 0b00000100;		//NS
	if(mad_GPS_DATA.EW == true)		mad_GPS_DATA_MEM[1] += 0b00000010;		//EW

	//HHMMSS
	tmp[0] = mad_GPS_DATA.TIME[0];	//hh = 5bit
	tmp[1] = mad_GPS_DATA.TIME[1];
	tmp[2] = 0x00;
	mad_GPS_DATA_MEM[1] += 	((atoi(tmp) >> 4) & 0b00000001);				//hour
	mad_GPS_DATA_MEM[2] = 	((atoi(tmp) << 4) & 0b11110000);
	tmp[0] = mad_GPS_DATA.TIME[2];	//mm = 6bit
	tmp[1] = mad_GPS_DATA.TIME[3];
	mad_GPS_DATA_MEM[2] += 	((atoi(tmp) >> 2) & 0b00001111);				//min
	mad_GPS_DATA_MEM[3] = 	((atoi(tmp) << 6) & 0b11000000);				//
	tmp[0] = mad_GPS_DATA.TIME[4];	//ss = 6bit
	tmp[1] = mad_GPS_DATA.TIME[5];
	mad_GPS_DATA_MEM[3] += 	(atoi(tmp) & 0b00111111);						//sec

	//ddmmyy
	tmp[0] = mad_GPS_DATA.DATE[4];	//yy=7bit
	tmp[1] = mad_GPS_DATA.DATE[5];
	tmp[2] = 0x00;
	mad_GPS_DATA_MEM[4] = 	(atoi(tmp) << 1);								//YEAR
	tmp[0] = mad_GPS_DATA.DATE[2];	//mm = 4bit
	tmp[1] = mad_GPS_DATA.DATE[3];
	mad_GPS_DATA_MEM[4] += 	((atoi(tmp) >> 3) & 0b00000001);				//MONTH
	mad_GPS_DATA_MEM[5] = 	((atoi(tmp) << 5) & 0b11100000);				//MONTH
	tmp[0] = mad_GPS_DATA.DATE[0];	//dd =5bit
	tmp[1] = mad_GPS_DATA.DATE[1];
	mad_GPS_DATA_MEM[5] += 	(atoi(tmp) & 0b00011111);						//DAY

	//LAT
	mad_GPS_DATA_MEM[6] =  	(mad_GPS_INT.LAT >>20)& 0xFF;						//LAT
	mad_GPS_DATA_MEM[7] =  	(mad_GPS_INT.LAT >>12)& 0xFF;
	mad_GPS_DATA_MEM[8] =  	(mad_GPS_INT.LAT >>4)& 0xFF;
	mad_GPS_DATA_MEM[9] =  	(mad_GPS_INT.LAT <<4 )& 0xF0;
	//LON
	mad_GPS_DATA_MEM[9] += 	(mad_GPS_INT.LON >>24)& 0x0F;						//LAT
	mad_GPS_DATA_MEM[10] = 	(mad_GPS_INT.LON >>16)& 0xFF;
	mad_GPS_DATA_MEM[11] = 	(mad_GPS_INT.LON >>8 )& 0xFF;
	mad_GPS_DATA_MEM[12] = 	(mad_GPS_INT.LON     )& 0xFF;
	//ALT

	int tmp32 = mad_GPS_INT.ALT;												//���x�̕��������
	if(tmp32 < 0)tmp32 = tmp32 * -1;
	mad_GPS_DATA_MEM[13] = 	(tmp32 >>8 )& 0x3F;
	mad_GPS_DATA_MEM[14] = 	tmp32 & 0xFF;
	//DOP
	mad_GPS_DATA_MEM[15] = 	mad_GPS_INT.DOP;

/*	���������p�f�[�^
   01234567
 0 XXXXXXXX
 1  GGGGhhH
 2 HHHHMMMM
 3 MMSSSSSS
 4 yyyyyyym
 5 mmmddddd
 6 CCCCCCCC
 7 CCCCCCCC
 8 CCCCCCCC
 9 CCCCDDDD
10 DDDDDDDD
11 DDDDDDDD
12 DDDDDDDD
13   EEEEEE
14 EEEEEEEE
15 FFFFFFFF

Status	X	0-25		 8 bit
�g�p�q����	G	0-16		 4 bit
��		HH	00-23		 5 bit
��		MM	00-59		 5 bit
�b		SS	00-59		 6 bit
�N		YY	00-99		 9 bit
��		MM	01-12		 4 bit
��		DD	01-31		 5 bit
�ܓx		C	89.999999	28 bit
�o�x		D	179.999999	28 bit
���x		E	0-16384		14 bit
HDOP	F	0-255 		 8 bit		//���ۂ�HDOP�l��10�{

NS, EW	N/E=0, S/W=1 2 bit

*/
}
//--------------------------------------------------------------------------------------------
void	mad_GPS_Mem2Int(void){

	if(mad_GPS_INT.SAT >= 16)mad_GPS_DATA_MEM[1] = 0b01111000;
	else mad_GPS_INT.SAT = (mad_GPS_DATA_MEM[1] >> 3) & 0x0F;
	if((mad_GPS_DATA_MEM[1] & 0b00000100) != 0x00)mad_GPS_INT.NS = true;	else mad_GPS_INT.NS = false;
	if((mad_GPS_DATA_MEM[1] & 0b00000010) != 0x00)mad_GPS_INT.EW = true;	else mad_GPS_INT.EW = false;

	int hour,min,sec;
	hour = ((mad_GPS_DATA_MEM[1] & 0x01) << 4 ) + ((mad_GPS_DATA_MEM[2] >> 4) & 0x0F);
	min  = ((mad_GPS_DATA_MEM[2] & 0x0F) << 2 ) + ((mad_GPS_DATA_MEM[3] >> 6) & 0x03);
	sec  = mad_GPS_DATA_MEM[3] & 0x03F;
	mad_GPS_INT.TIME = (hour * 10000) + (min * 100) + sec;

	int year, month, day;
	year  = (mad_GPS_DATA_MEM[4] >> 1) & 0x7F;
	month = ((mad_GPS_DATA_MEM[4] << 3 & 0x08)) + ((mad_GPS_DATA_MEM[5] >> 5) & 0x7);
	day   = mad_GPS_DATA_MEM[5] & 0x01F;
	mad_GPS_INT.DATE = (day * 10000) + (month * 100) + year;


	mad_GPS_INT.LAT  = mad_GPS_DATA_MEM[6] * 0x100000;
	mad_GPS_INT.LAT += mad_GPS_DATA_MEM[7] * 0x001000;
	mad_GPS_INT.LAT += mad_GPS_DATA_MEM[8] * 0x000010;
	mad_GPS_INT.LAT += ((mad_GPS_DATA_MEM[9] >> 4) & 0x0F);

	mad_GPS_INT.LON  = (mad_GPS_DATA_MEM[9] & 0x0f) * 256*256*256;
	mad_GPS_INT.LON +=  mad_GPS_DATA_MEM[10] * 256*256;
	mad_GPS_INT.LON +=  mad_GPS_DATA_MEM[11] * 256;
	mad_GPS_INT.LON +=  mad_GPS_DATA_MEM[12];

	mad_GPS_INT.ALT =  (mad_GPS_DATA_MEM[13] & 0x3F) * 0x100;
	mad_GPS_INT.ALT +=  mad_GPS_DATA_MEM[14];

	if((mad_GPS_DATA_MEM[1] & 0x80) == 0x80)mad_GPS_INT.ALT = mad_GPS_INT.ALT * -1;

	mad_GPS_INT.DOP = mad_GPS_DATA_MEM[15];
}
//--------------------------------------------------------------------------------------------
void	mad_GPS_TrueMode(void)
{
  	mad_GPS_ON();	//ON���Ă��珉��������܂ł̊Ԃ�WAIT���K�v
	mad_TIMER1_WAIT_10ms(100);
	mad_USART0_INIT(9600);			//PC
	mad_USART1_INIT(9600);			//ZOE-M8G

	while(1){
		if(	mad_UART0_RX_BUF.empty == false){
			mad_USART1_TxChar(mad_UART0_RX_BUF.data[0]);
			mad_USART0_RxBufClr();
		}
		if(	mad_UART1_RX_BUF.empty == false){
			mad_USART0_TxChar(mad_UART1_RX_BUF.data[0]);
			mad_USART1_RxBufClr();
		}
	}
}
//--------------------------------------------------------------------------------------------
void	mad_GPS_BufClr(void)
{
	mad_USART1_RxBufClr();

	int i;
	for(i = 0; i< 10; i++)		mad_GPS_DATA.TIME[i]= 0x00;
	for(i = 0; i< 11; i++)		mad_GPS_DATA.LAT[i]= 0x00;
	mad_GPS_DATA.NS = false;
	for(i = 0; i< 12; i++)		mad_GPS_DATA.LON[i]= 0x00;
	mad_GPS_DATA.EW = false;
	for(i = 0; i< 5; i++)		mad_GPS_DATA.SAT[i]= 0x00;
	for(i = 0; i< 5; i++)		mad_GPS_DATA.DOP[i]= 0x00;
	for(i = 0; i< 10; i++)		mad_GPS_DATA.ALT[i]= 0x00;
//	for(i = 0; i< 10; i++)		mad_GPS_DATA.TIME[i]= 0x00;
//	for(i = 0; i< 10; i++)		mad_GPS_DATA.TIME[i]= 0x00;
	for(i = 0; i< 10; i++)		mad_GPS_DATA.DATE[i]= 0x00;

	mad_GPS_DATA.RCVD = RCVD_NONE;
//	mad_GPS_DATA.GGA_Recvd = false;
//	mad_GPS_DATA.RMC_Recvd = false;
	mad_GPS_DATA.Error = false;



}
//--------------------------------------------------------------------------------------------
void	mad_GPS_ON(void)
{
	mad_GPIO_Set(mad_GPIO_GPS_ON);
}
//--------------------------------------------------------------------------------------------
void	mad_GPS_OFF(void)
{
  	mad_GPIO_Clr(mad_GPIO_GPS_ON);
	// GPIO_PinModeSet(gpioPortD, 9, gpioModeDisabled, 1);		//TxD
	// GPIO_PinModeSet(gpioPortD, 10, gpioModeDisabled, 0);		//RxD
}
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
