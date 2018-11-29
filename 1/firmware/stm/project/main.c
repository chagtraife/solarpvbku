#include "stm32f10x.h"
#include <string.h>
#include "stdio.h"
#include <math.h>
#include <stdlib.h>
#include "stm32f10x_usart.h"

/*//=======================================================
CT1-A6         VT1-A5
CT2-A4         VT2-A3
CT3-A2         VT3-A1

CT: I_CT1 = V_A6-3.3/2
		I_CT2 = V_A4-3.3/2
		I_CT3 = V_A2-3.3/2

VT: VT1-A5 = V_A5*scale - 3.3/2           //scale = calib*(R1+R2)/R2     : n:ti so bien ap 220/12; R1 = 220k; R2 = 10k
					 = V_A%*23 - 3.3/2
//-=======================================================*/
#define GPIO_CT1 GPIO_Pin_6
#define GPIO_CT2 GPIO_Pin_4
#define GPIO_CT3 GPIO_Pin_2
#define GPIO_VT1 GPIO_Pin_5
#define GPIO_VT2 GPIO_Pin_3
#define GPIO_VT3 GPIO_Pin_1
//----------------------------------------------------------------
#define ADC_CT1 ADC_Channel_6
#define ADC_CT2 ADC_Channel_4
#define ADC_CT3 ADC_Channel_2
#define ADC_VT1 ADC_Channel_5
#define ADC_VT2 ADC_Channel_3
#define ADC_VT3 ADC_Channel_1
//===============================================================

#define SLAVE_ID  01
//#define TEST
#define MEA_STATE    3
#define RECV_STATE   2
#define TRANS_STATE  1
#define IDLE_STATE   0





#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#endif /* __GNUC__ */
	
PUTCHAR_PROTOTYPE
{
	/* Place your implementation of fputc here */
	//lcd_Data_Write((u8)ch);
	USART_SendData(USART1,(u8)ch);
	//HAL_UART_Transmit(&huart2, (uint8_t*)&ch,1,100);
	
	/*Loop until the end of transmission */
	while (USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET)
	{}

  	return ch;
}





/*Khoi tao bien cau hinh*/
GPIO_InitTypeDef			GPIO_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
USART_InitTypeDef			UART_InitStructure;
ADC_InitTypeDef 			ADC_InitStructure;
ADC_InitTypeDef       ADC1_InitStructure;
ADC_InitTypeDef				ADC2_InitStructure;
TIM_TimeBaseInitTypeDef TIM_InitStructure;



typedef struct ct
	{
		int iadc[250];
		float i[250];
		int len_iadc;
		float I;
		float Iavr;//(mA)
		}ct;
typedef struct vt
	{
		int uadc[250];
		float u[250];
		int len_uadc;
		float U;//(0.1V)
		float Uavr;
		}vt;
typedef struct p
	{
		float P;
		float Pavr;//(0.0001W)
		
		}p;

ct ct1,ct2,ct3;
vt vt1,vt2,vt3;
p p1,p2,p3;
		
		
		
//uint8_t data =10;
float value_adc1 = 0,sum_adc1=0, adc_tb1=0;
float value_adc2 = 0,sum_adc2=0, adc_tb2=0;
int k,r;



#ifdef TEST
	int uadc_2[500] = {2048,	2072,	2097,	2121,	2145,	2169,	2194,	2218,	2242,	2266,	2290,	2314,	2338,	2362,	2386,	2410,	2434,	2458,	2481,	2505,	2529,	2552,	2576,	2599,	2622,	2645,	2668,	2691,	2714,	2737,	2759,	2782,	2804,	2827,	2849,	2871,	2893,	2915,	2936,	2958,	2979,	3000,	3021,	3042,	3063,	3083,	3104,	3124,	3144,	3164,	3184,	3203,	3223,	3242,	3261,	3280,	3298,	3317,	3335,	3353,	3371,	3388,	3406,	3423,	3440,	3456,	3473,	3489,	3505,	3521,	3536,	3552,	3567,	3582,	3596,	3611,	3625,	3639,	3652,	3666,	3679,	3692,	3704,	3717,	3729,	3740,	3752,	3763,	3774,	3785,	3795,	3805,	3815,	3825,	3834,	3843,	3852,	3860,	3869,	3876,	3884,	3891,	3898,	3905,	3912,	3918,	3923,	3929,	3934,	3939,	3944,	3948,	3952,	3956,	3959,	3962,	3965,	3968,	3970,	3972,	3974,	3975,	3976,	3977,	3977,	3977,	3977,	3976,	3975,	3974,	3973,	3971,	3969,	3967,	3964,	3961,	3958,	3954,	3950,	3946,	3942,	3937,	3932,	3926,	3921,	3915,	3908,	3902,	3895,	3888,	3880,	3873,	3865,	3856,	3848,	3839,	3830,	3820,	3810,	3800,	3790,	3780,	3769,	3758,	3746,	3735,	3723,	3710,	3698,	3685,	3672,	3659,	3646,	3632,	3618,	3604,	3589,	3574,	3559,	3544,	3529,	3513,	3497,	3481,	3465,	3448,	3431,	3414,	3397,	3379,	3362,	3344,	3326,	3307,	3289,	3270,	3251,	3232,	3213,	3194,	3174,	3154,	3134,	3114,	3094,	3073,	3053,	3032,	3011,	2990,	2968,	2947,	2925,	2904,	2882,	2860,	2838,	2816,	2793,	2771,	2748,	2725,	2703,	2680,	2657,	2634,	2610,	2587,	2564,	2540,	2517,	2493,	2470,	2446,	2422,	2398,	2374,	2350,	2326,	2302,	2278,	2254,	2230,	2206,	2181,	2157,	2133,	2109,	2084,	2060,	2036,	2012,	1987,	1963,	1939,	1915,	1890,	1866,	1842,	1818,	1794,	1770,	1746,	1722,	1698,	1674,	1650,	1626,	1603,	1579,	1556,	1532,	1509,	1486,	1462,	1439,	1416,	1393,	1371,	1348,	1325,	1303,	1280,	1258,	1236,	1214,	1192,	1171,	1149,	1128,	1106,	1085,	1064,	1043,	1023,	1002,	982,	962,	942,	922,	902,	883,	864,	845,	826,	807,	789,	770,	752,	734,	717,	699,	682,	665,	648,	631,	615,	599,	583,	567,	552,	537,	522,	507,	492,	478,	464,	450,	437,	424,	411,	398,	386,	373,	361,	350,	338,	327,	316,	306,	296,	286,	276,	266,	257,	248,	240,	231,	223,	216,	208,	201,	194,	188,	181,	175,	170,	164,	159,	154,	150,	146,	142,	138,	135,	132,	129,	127,	125,	123,	122,	121,	120,	119,	119,	119,	119,	120,	121,	122,	124,	126,	128,	131,	134,	137,	140,	144,	148,	152,	157,	162,	167,	173,	178,	184,	191,	198,	205,	212,	220,	227,	236,	244,	253,	262,	271,	281,	291,	301,	311,	322,	333,	344,	356,	367,	379,	392,	404,	417,	430,	444,	457,	471,	485,	500,	514,	529,	544,	560,	575,	591,	607,	623,	640,	656,	673,	690,	708,	725,	743,	761,	779,	798,	816,	835,	854,	873,	893,	912,	932,	952,	972,	992,	1013,	1033,	1054,	1075,	1096,	1117,	1138,	1160,	1181,	1203,	1225,	1247,	1269,	1292,	1314,	1337,	1359,	1382,	1405,	1428,	1451,	1474,	1497,	1520,	1544,	1567,	1591,	1615,	1638,	1662,	1686,	1710,	1734,	1758,	1782,	1806,	1830,	1854,	1878,	1902,	1927,	1951,	1975,	1999,	2024,	2048};
  int iadc_1[500] = {2048,	2072,	2097,	2121,	2145,	2169,	2194,	2218,	2242,	2266,	2290,	2314,	2338,	2362,	2386,	2410,	2434,	2458,	2481,	2505,	2529,	2552,	2576,	2599,	2622,	2645,	2668,	2691,	2714,	2737,	2759,	2782,	2804,	2827,	2849,	2871,	2893,	2915,	2936,	2958,	2979,	3000,	3021,	3042,	3063,	3083,	3104,	3124,	3144,	3164,	3184,	3203,	3223,	3242,	3261,	3280,	3298,	3317,	3335,	3353,	3371,	3388,	3406,	3423,	3440,	3456,	3473,	3489,	3505,	3521,	3536,	3552,	3567,	3582,	3596,	3611,	3625,	3639,	3652,	3666,	3679,	3692,	3704,	3717,	3729,	3740,	3752,	3763,	3774,	3785,	3795,	3805,	3815,	3825,	3834,	3843,	3852,	3860,	3869,	3876,	3884,	3891,	3898,	3905,	3912,	3918,	3923,	3929,	3934,	3939,	3944,	3948,	3952,	3956,	3959,	3962,	3965,	3968,	3970,	3972,	3974,	3975,	3976,	3977,	3977,	3977,	3977,	3976,	3975,	3974,	3973,	3971,	3969,	3967,	3964,	3961,	3958,	3954,	3950,	3946,	3942,	3937,	3932,	3926,	3921,	3915,	3908,	3902,	3895,	3888,	3880,	3873,	3865,	3856,	3848,	3839,	3830,	3820,	3810,	3800,	3790,	3780,	3769,	3758,	3746,	3735,	3723,	3710,	3698,	3685,	3672,	3659,	3646,	3632,	3618,	3604,	3589,	3574,	3559,	3544,	3529,	3513,	3497,	3481,	3465,	3448,	3431,	3414,	3397,	3379,	3362,	3344,	3326,	3307,	3289,	3270,	3251,	3232,	3213,	3194,	3174,	3154,	3134,	3114,	3094,	3073,	3053,	3032,	3011,	2990,	2968,	2947,	2925,	2904,	2882,	2860,	2838,	2816,	2793,	2771,	2748,	2725,	2703,	2680,	2657,	2634,	2610,	2587,	2564,	2540,	2517,	2493,	2470,	2446,	2422,	2398,	2374,	2350,	2326,	2302,	2278,	2254,	2230,	2206,	2181,	2157,	2133,	2109,	2084,	2060,	2036,	2012,	1987,	1963,	1939,	1915,	1890,	1866,	1842,	1818,	1794,	1770,	1746,	1722,	1698,	1674,	1650,	1626,	1603,	1579,	1556,	1532,	1509,	1486,	1462,	1439,	1416,	1393,	1371,	1348,	1325,	1303,	1280,	1258,	1236,	1214,	1192,	1171,	1149,	1128,	1106,	1085,	1064,	1043,	1023,	1002,	982,	962,	942,	922,	902,	883,	864,	845,	826,	807,	789,	770,	752,	734,	717,	699,	682,	665,	648,	631,	615,	599,	583,	567,	552,	537,	522,	507,	492,	478,	464,	450,	437,	424,	411,	398,	386,	373,	361,	350,	338,	327,	316,	306,	296,	286,	276,	266,	257,	248,	240,	231,	223,	216,	208,	201,	194,	188,	181,	175,	170,	164,	159,	154,	150,	146,	142,	138,	135,	132,	129,	127,	125,	123,	122,	121,	120,	119,	119,	119,	119,	120,	121,	122,	124,	126,	128,	131,	134,	137,	140,	144,	148,	152,	157,	162,	167,	173,	178,	184,	191,	198,	205,	212,	220,	227,	236,	244,	253,	262,	271,	281,	291,	301,	311,	322,	333,	344,	356,	367,	379,	392,	404,	417,	430,	444,	457,	471,	485,	500,	514,	529,	544,	560,	575,	591,	607,	623,	640,	656,	673,	690,	708,	725,	743,	761,	779,	798,	816,	835,	854,	873,	893,	912,	932,	952,	972,	992,	1013,	1033,	1054,	1075,	1096,	1117,	1138,	1160,	1181,	1203,	1225,	1247,	1269,	1292,	1314,	1337,	1359,	1382,	1405,	1428,	1451,	1474,	1497,	1520,	1544,	1567,	1591,	1615,	1638,	1662,	1686,	1710,	1734,	1758,	1782,	1806,	1830,	1854,	1878,	1902,	1927,	1951,	1975,	1999,	2024,	2048};
	int iadc_2[500] = {2048,	2064,	2080,	2097,	2113,	2129,	2145,	2161,	2177,	2193,	2209,	2226,	2242,	2258,	2274,	2289,	2305,	2321,	2337,	2353,	2368,	2384,	2400,	2415,	2431,	2446,	2462,	2477,	2492,	2507,	2522,	2537,	2552,	2567,	2582,	2597,	2611,	2626,	2640,	2654,	2669,	2683,	2697,	2711,	2725,	2738,	2752,	2765,	2779,	2792,	2805,	2818,	2831,	2844,	2857,	2869,	2881,	2894,	2906,	2918,	2930,	2941,	2953,	2964,	2976,	2987,	2998,	3009,	3019,	3030,	3040,	3051,	3061,	3071,	3080,	3090,	3099,	3108,	3118,	3126,	3135,	3144,	3152,	3160,	3168,	3176,	3184,	3191,	3199,	3206,	3213,	3220,	3226,	3233,	3239,	3245,	3251,	3256,	3262,	3267,	3272,	3277,	3282,	3286,	3290,	3294,	3298,	3302,	3305,	3309,	3312,	3315,	3317,	3320,	3322,	3324,	3326,	3328,	3329,	3331,	3332,	3333,	3333,	3334,	3334,	3334,	3334,	3333,	3333,	3332,	3331,	3330,	3329,	3327,	3325,	3323,	3321,	3319,	3316,	3313,	3310,	3307,	3304,	3300,	3296,	3292,	3288,	3284,	3279,	3275,	3270,	3264,	3259,	3253,	3248,	3242,	3236,	3229,	3223,	3216,	3209,	3202,	3195,	3188,	3180,	3172,	3164,	3156,	3148,	3139,	3131,	3122,	3113,	3104,	3095,	3085,	3075,	3066,	3056,	3045,	3035,	3025,	3014,	3003,	2992,	2981,	2970,	2959,	2947,	2936,	2924,	2912,	2900,	2888,	2875,	2863,	2850,	2838,	2825,	2812,	2799,	2785,	2772,	2759,	2745,	2731,	2718,	2704,	2690,	2676,	2662,	2647,	2633,	2618,	2604,	2589,	2575,	2560,	2545,	2530,	2515,	2500,	2484,	2469,	2454,	2438,	2423,	2407,	2392,	2376,	2361,	2345,	2329,	2313,	2297,	2281,	2266,	2250,	2234,	2218,	2201,	2185,	2169,	2153,	2137,	2121,	2105,	2088,	2072,	2056,	2040,	2024,	2008,	1991,	1975,	1959,	1943,	1927,	1911,	1895,	1878,	1862,	1846,	1830,	1815,	1799,	1783,	1767,	1751,	1735,	1720,	1704,	1689,	1673,	1658,	1642,	1627,	1612,	1596,	1581,	1566,	1551,	1536,	1521,	1507,	1492,	1478,	1463,	1449,	1434,	1420,	1406,	1392,	1378,	1365,	1351,	1337,	1324,	1311,	1297,	1284,	1271,	1258,	1246,	1233,	1221,	1208,	1196,	1184,	1172,	1160,	1149,	1137,	1126,	1115,	1104,	1093,	1082,	1071,	1061,	1051,	1040,	1030,	1021,	1011,	1001,	992,	983,	974,	965,	957,	948,	940,	932,	924,	916,	908,	901,	894,	887,	880,	873,	867,	860,	854,	848,	843,	837,	832,	826,	821,	817,	812,	808,	804,	800,	796,	792,	789,	786,	783,	780,	777,	775,	773,	771,	769,	767,	766,	765,	764,	763,	763,	762,	762,	762,	762,	763,	763,	764,	765,	767,	768,	770,	772,	774,	776,	779,	781,	784,	787,	791,	794,	798,	802,	806,	810,	814,	819,	824,	829,	834,	840,	845,	851,	857,	863,	870,	876,	883,	890,	897,	905,	912,	920,	928,	936,	944,	952,	961,	970,	978,	988,	997,	1006,	1016,	1025,	1035,	1045,	1056,	1066,	1077,	1087,	1098,	1109,	1120,	1132,	1143,	1155,	1166,	1178,	1190,	1202,	1215,	1227,	1239,	1252,	1265,	1278,	1291,	1304,	1317,	1331,	1344,	1358,	1371,	1385,	1399,	1413,	1427,	1442,	1456,	1470,	1485,	1499,	1514,	1529,	1544,	1559,	1574,	1589,	1604,	1619,	1634,	1650,	1665,	1681,	1696,	1712,	1728,	1743,	1759,	1775,	1791,	1807,	1822,	1838,	1854,	1870,	1887,	1903,	1919,	1935,	1951,	1967,	1983,	1999,	2016,	2032,	2048};	
	//r1(iadc_1) =10 ohm
	//r2(iadc_2) =15 ohm	
#endif
int len_iadc_ct1, len_iadc_ct2, len_iadc_ct3, len_uadc_vt1, len_uadc_vt2, len_uadc_vt3;
float I1, I2, I3, U1, U2, U3, P1, P2, P3, PF;

	
	

volatile extern char state;
volatile extern char cmd_buf[8];

void measure_data(void);
void avr_data(void);

void GPIO_Configuration(void);
void Delay_ms(uint16_t time);
void Delay_us(uint16_t time);
void UART_Configuration (void);
void ADC_Configuration(void);
int read_adc(ADC_TypeDef* ADCx,uint8_t ADC_Channel);// adc-> voltage
float convert_adc(float value_adc, int scale);
float AVR_cal(float *value, int len_value);// caculator AVR
float P_CAL(float *value1,float *value2, int len_value);//caculator  Potestas  (w)
//float PF_cal();//caculator POWER FACTOR
//float f_cal();//caculator frenquence(Hz)
unsigned int calcuteCRC16 (unsigned char *pBuff, unsigned char pLen);
void send_data(void);
void process_cmd(void);

int main(void)
{

	GPIO_Configuration();
	UART_Configuration();//uart_debug+ uart to ras
	ADC_Configuration();// CT1/CT2/CT3/ VT1/VT2/VT3
	state  = MEA_STATE;
	r=0;// l: la so lan MEAS trong khoang thoi gian request cua esp
  while (1)
  {	
		switch(state)
			{
			case IDLE_STATE:
				{
					//PWR_WakeUpPinCmd(ENABLE);
					//PWR_DeInit();
          //PWR_EnterSTANDBYMode();
					//state = MEA_STATE;
					break;
					}
			case TRANS_STATE: 
				{
					send_data();
					state = MEA_STATE;
					r=0;
					break;
					}
			case RECV_STATE:
				{
			//		process_cmd();
					break;
					}
			case MEA_STATE:
				{
					measure_data(); // mesure data trong 1 chu ki dien
					r++;
					avr_data();
				//	state = TRANS_STATE;
					break;
					}
			}
					//send data to ras	
			//	Delay_ms(1000);
					
  }
}
void GPIO_Configuration(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}
void Delay_ms(uint16_t time)
{
uint32_t time_n=time*12000;
	while(time_n!=0){time_n--;}
}

void Delay_us(uint16_t time)
 {
uint32_t time_n=time*12;
	while(time_n!=0){time_n--;}
}
 

void UART_Configuration (void)
	{
		/*Cap clock cho USART và port su dung*/
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
		
			/* Cau Tx mode AF_PP, Rx mode FLOATING  */
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;//TX
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(GPIOA, &GPIO_InitStructure);
		
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//RX
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
			GPIO_Init(GPIOA, &GPIO_InitStructure);	
		
		
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    /* Enable the USARTy Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
		
		/*Cau hinh USART*/
			UART_InitStructure.USART_BaudRate = 9600;
			UART_InitStructure.USART_WordLength = USART_WordLength_8b;
			UART_InitStructure.USART_StopBits = USART_StopBits_1;
			UART_InitStructure.USART_Parity = USART_Parity_No;
			UART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
			UART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
			USART_Init(USART1, &UART_InitStructure);
				
			/* Cho phep UART hoat dong */
			USART_Cmd(USART1, ENABLE);
	    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //cho phep ngat nhan 
      //USART_ITConfig(USART1, USART_IT_TXE, ENABLE); // cho phep ngat truyen
	}

void ADC_Configuration(void)
	{
		/*cap clock cho chan GPIO va bo ADC1*/
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 , ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		
		/*cau hinh chan Input cua bo ADC1 la chan PA6==CT1*/
		GPIO_InitStructure.GPIO_Pin =  GPIO_CT1;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		
				/*cau hinh chan Input cua bo ADC1 la chan PA4==CT2*/
		GPIO_InitStructure.GPIO_Pin =  GPIO_CT2;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

				/*cau hinh chan Input cua bo ADC1 la chan PA2==CT3*/
		GPIO_InitStructure.GPIO_Pin =  GPIO_CT3;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		/*cau hinh chan Input cua bo ADC1 la chan PA5==VT1*/
		GPIO_InitStructure.GPIO_Pin =  GPIO_VT1;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		
				/*cau hinh chan Input cua bo ADC1 la chan PA3==VT2*/
		GPIO_InitStructure.GPIO_Pin =  GPIO_VT2;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

				/*cau hinh chan Input cua bo ADC1 la chan PA1==VT3*/
		GPIO_InitStructure.GPIO_Pin =  GPIO_VT3;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		

		/*cau hinh ADC1*/
		ADC1_InitStructure.ADC_Mode = ADC_Mode_Independent;
		ADC1_InitStructure.ADC_ScanConvMode = ENABLE;
		ADC1_InitStructure.ADC_ContinuousConvMode = ENABLE;
		ADC1_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
		ADC1_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
		ADC1_InitStructure.ADC_NbrOfChannel = 1;
		ADC_Init(ADC1, &ADC1_InitStructure);
	
	  /* Enable ADC1 DMA */
	  ADC_DMACmd(ADC1, ENABLE);
//		/* Cau hinh chanel, rank, thoi gian lay mau */
//		ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
			/* Cho phep bo ADC1 hoa dong */
		ADC_Cmd(ADC1, ENABLE);   
//		/* cho phep cam bien nhiet hoat dong */
//		ADC_TempSensorVrefintCmd(ENABLE);
		/* Reset thanh ghi cablib  */   
		ADC_ResetCalibration(ADC1);
		/* Cho thanh ghi cablib reset xong */
		while(ADC_GetResetCalibrationStatus(ADC1));
		/* Khoi dong bo ADC */
		ADC_StartCalibration(ADC1);	
		/* Cho trang thai cablib duoc bat */
		while(ADC_GetCalibrationStatus(ADC1));
		/* Bat dau chuyen doi ADC */ 
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);

	}
	
	
int	read_adc(ADC_TypeDef* ADCx,uint8_t ADC_Channel)
	  {
    int result,value_adc;
		value_adc =0;
		    ADC_RegularChannelConfig(ADCx, ADC_Channel, 1 , ADC_SampleTime_239Cycles5);
			//Delay_us(100);
		    value_adc = ADC_GetConversionValue(ADCx);

			  result = value_adc;
		    return result;
	}

float convert_adc(float value_adc, int scale)
	{
		float result ;
		result =( (value_adc-2048)/1240 )*scale;
		return result;
		}

float AVR_cal(float	*value, int len_value)// caculator RMS
	{
		float result;
		result =0;
		int k;
		for(k=0;k < len_value ; k++)
		{
			result = result + (*(value+k));
			}
		result = result/k;// result = result / len_value
	return result;
		}
float P_CAL(float *value1,float *value2, int len_value)//caculator  Potestas  (w)
{
	float result;
	int k;
	for(k=0; k<len_value; k++)
	{
		result = result + (*(value1+k))*(*(value2+k));
		}
	result = result *0.002;// result = result / len_value
	//result = value1*value2*0.002;
	return result;
	}
//float PF_cal()//caculator POWER FACTOR
//	{
//		float result;
//    return result;
//		}
//float f_cal()//caculator frenquence(Hz)
//{
//    float result;
//	  return result;
//}

//----------------------------------------------------------------------------------------------

static unsigned int calcuteCRC16 (unsigned char *pBuff, unsigned char pLen)
{
    unsigned int crc = 0xFFFF;
    int pos,i;
      for ( pos = 0; pos < pLen; pos++)
      {
        crc ^= (unsigned int) pBuff[pos]; // XOR byte into least sig. byte of crc
        for ( i = 0; i < 8; i++)   // Loop over each bit
        {
          if ((crc & 0x0001) != 0 )   // If the LSB is set
          {
            crc >>= 1;
            crc ^= 0xA001;
          }
          else
            crc >>= 1;
        };
      };
      // Note, this number has low and high bytes swapped, so use it accordingly (or swap bytes)
      return crc;
}
//----------------------------------------------------------------------------------------------	

void send_data(void)
{
	//		printf("send data");
	printf("?node=solarpvtxdc&fulljson={\"I1_mA\":%d,\"I2_mA\":%d,\"I3_mA\":%d,", (int) ct1.Iavr, (int) ct2.Iavr,(int) ct3.Iavr);
	printf("\"U1_0.1V\":%d,\"U2_0.1V\":%d,\"U3_0.1V\":%d,",(int) vt1.Uavr,(int) vt2.Uavr,(int) vt3.Uavr);
	printf("\"P1_mW\":%d,\"P2_mW\":%d,\"P3_mW\":%d}\n",(int) (p1.Pavr/10),(int) (p2.Pavr/10),(int) (p3.Pavr/10));
	
	//I1,I2,I3:mA;    U1,U2,U3:0.1V;      P1,P2,P3: mW      
	
}
void process_cmd(void)
{
	unsigned int CRC16;
	union check_crc{
        unsigned int crc_full;
        struct {
            unsigned char crc_lsb;
            unsigned char crc_msb;
        }crc_half ;
    } checkcrc;
	unsigned char *pBuff;
	pBuff =(unsigned char *) cmd_buf;
	CRC16 = calcuteCRC16(pBuff,8);	
  checkcrc.crc_half.crc_msb=*(pBuff+8);
  checkcrc.crc_half.crc_lsb=*(pBuff+7);		
	if (CRC16 !=	checkcrc.crc_full) return ;
	
}
void measure_data(void)
	{
			//printf("measure data");
		#ifndef TEST
					
					k=-1;
					//get data
					  do
					{
								k++;
								ct1.iadc[k] = read_adc(ADC1, ADC_CT1);//iadc_ct1
							  ct2.iadc[k] = read_adc(ADC1, ADC_CT2);//iadc_ct2
						    ct3.iadc[k] = read_adc(ADC1, ADC_CT3);//iadc_ct3
								vt1.uadc[k] = read_adc(ADC1, ADC_VT1);//uadc_vt1
						    vt2.uadc[k] = read_adc(ADC1, ADC_VT2);//uadc_vt2
						    vt3.uadc[k] = read_adc(ADC1, ADC_VT3);//uadc_vt3
								//Delay_ms(1);
					}while(!(k==240)); 		//20ms = 40us*500
					#else
					k= 490;
	  #endif
					
					
					
					int m=0;
					//convert voltage adc -> current/voltage true
					for(m=0;m<k+1;m++)
					{
					ct1.i[m] = convert_adc(ct1.iadc[m],1000);     //1->A; 1000->mA
					ct2.i[m] = convert_adc(ct2.iadc[m],1000);
					ct3.i[m] = convert_adc(ct3.iadc[m],1000);
					vt1.u[m] = convert_adc(vt1.uadc[m],230);// scale = calib*n*(R1+R2)/R2     : n:ti so bien ap 220/12; R1 = 220k; R2 = 10k
					vt2.u[m] = convert_adc(vt2.uadc[m],230);//23->V
					vt3.u[m] = convert_adc(vt3.uadc[m],230);
					}
					
					
					//caculator I_RMS, U_RMS, P, PF, frequence
					ct1.I= AVR_cal(ct1.i,k+1);
					ct2.I= AVR_cal(ct2.i,k+1);
					ct3.I= AVR_cal(ct3.i,k+1);
					vt1.U= AVR_cal(vt1.u,k+1);
					vt2.U= AVR_cal(vt2.u,k+1);
					vt3.U= AVR_cal(vt3.u,k+1);
					
					/*//--------------
					for(m=0;m<k+1;m++)
					{
					printf("%f\n",u_2[m]);
						}
					//--------------*/	
						
//					f = f_cal();
					p1.P = P_CAL(ct1.i,vt1.u,k+1);
					p2.P = P_CAL(ct2.i,vt2.u,k+1);
					p3.P = P_CAL(ct3.i,vt3.u,k+1);
	//				PF = PF_cal();
		}
	
void avr_data(void)
	{

				ct1.Iavr = (ct1.Iavr*(r-1) + ct1.I)/r;
				ct2.Iavr = (ct3.Iavr*(r-1) + ct2.I)/r;
				ct3.Iavr = (ct3.Iavr*(r-1) + ct3.I)/r;
				vt1.Uavr = (vt1.Uavr*(r-1) + vt1.U)/r;
				vt2.Uavr = (vt2.Uavr*(r-1) + vt2.U)/r;
				vt3.Uavr = (vt3.Uavr*(r-1) + vt3.U)/r;
				p1.Pavr = (p1.Pavr*(r-1) + p1.P)/r;
				p2.Pavr = (p2.Pavr*(r-1) + p2.P)/r;
				p3.Pavr = (p3.Pavr*(r-1) + p3.P)/r;
				if(r>60000) {
					while(1){}
					}
		}
