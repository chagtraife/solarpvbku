#include "stm32f10x.h"
#include <string.h>
#include "stdio.h"
#include <math.h>
#include <stdlib.h>






/*//=======================================================
         VT1-A5
CT1-A4         
CT2-A2         

CT: I_CT1 = V_A4-3.3/2
		I_CT2 = V_A2-3.3/2

VT: VT1-A5 = V_A5*scale - 3.3/2           //scale = calib*(R1+R2)/R2     : n:ti so bien ap 220/12; R1 = 220k; R2 = 10k
					 = V_A%*23 - 3.3/2
//-=======================================================*/
#define GPIO_CT1 GPIO_Pin_4
#define GPIO_CT2 GPIO_Pin_2
#define GPIO_VT1 GPIO_Pin_5
//----------------------------------------------------------------
#define ADC_CT1 ADC_Channel_4
#define ADC_CT2 ADC_Channel_2
#define ADC_VT1 ADC_Channel_5
//===============================================================

typedef struct ct
	{
		uint16_t iadc[500];
		float i[500];
		int len_iadc;
		float I;
		float Iavr;//(mA)
		}ct;
typedef struct vt
	{
		uint16_t uadc[500];
		float u[500];
		int len_uadc;
		float U;//(0.1V)
		float Uavr;
		uint16_t time_f;
		float f;
		float favr;
		}vt;
typedef struct p
	{
		float P;
		float Pavr;//(0.0001W)
		float PF;
		float PFavr;
		}p;
extern int r,k,timeout;
volatile extern char state;
extern uint16_t ADC1ConvertedValue[3];
extern		GPIO_InitTypeDef  GPIO_InitStructure;
extern		ADC_InitTypeDef 			ADC1_InitStructure;
extern		DMA_InitTypeDef       DMA_InitStruct;


uint16_t read_adc(ADC_TypeDef* ADCx,uint8_t ADC_Channel);// adc-> voltage
void getdata_time(uint16_t time);
float convert_adc(uint16_t value_adc, int scale);
float RMS_cal(float *value, int len_value);// caculator RMS
float P_CAL(float *value1,float *value2, int len_value);//caculator  Potestas  (w)
float PF_cal(float u, float i, float p);//caculator POWER FACTOR
void measure_init(void);		
void measure_data_AC(void);
void avr_data(void);
void send_data(void);