#include "measure.h"
#include <stm32f10x_gpio.h>

		GPIO_InitTypeDef  GPIO_InitStructure;
		ADC_InitTypeDef 			ADC1_InitStructure;
		DMA_InitTypeDef       DMA_InitStruct;

ct ct1,ct2;
vt vtac;
p p1,p2;

int k,timeout,r;
uint16_t ADC1ConvertedValue[3] = {0,0,0};
volatile extern char state;

void TIM4_Configuration(void)
	{
		  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;


  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); // Enable TIM4 clock 
 
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);  // Time base configuration 

  TIM_TimeBaseStructure.TIM_Period = 65535;
  //TIM_TimeBaseStructure.TIM_Prescaler=(SystemCoreClock/DELAY_TIM_FREQUENCY)-1;
  TIM_TimeBaseStructure.TIM_Prescaler=0; //From examples...
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  //TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //Do not divide.
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

  TIM_PrescalerConfig(TIM4,(uint16_t) (SystemCoreClock/1000000)-1, TIM_PSCReloadMode_Immediate);   // Prescaler configuration

  TIM_Cmd(TIM4, ENABLE);   // Enable counter 
	}
void ADC_Configuration(void)
	{
		    /* Enable ADC1, DMA1 and GPIO clocks ****************************************/
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 , ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);//ADC1 is connected to the APB2 peripheral bus
		
		    /* DMA1  channel1 configuration **************************************/
    DMA_DeInit(DMA1_Channel1);
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;//ADC1's data register
    DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)&ADC1ConvertedValue;
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStruct.DMA_BufferSize = 3;
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//Reads 16 bit values
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//Stores 16 bit values
    DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStruct.DMA_Priority = DMA_Priority_High;
    DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
		DMA_Init(DMA1_Channel1, &DMA_InitStruct);
    DMA_Cmd(DMA1_Channel1, ENABLE);
		
		/*cau hinh chan Input cua bo ADC1 la chan CT1*/
		GPIO_InitStructure.GPIO_Pin =  GPIO_CT1;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		
				/*cau hinh chan Input cua bo ADC1 la chan CT2*/
		GPIO_InitStructure.GPIO_Pin =  GPIO_CT2;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);



		/*cau hinh chan Input cua bo ADC1 la chan PA5==VT1*/
		GPIO_InitStructure.GPIO_Pin =  GPIO_VT1;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		


		/*cau hinh ADC1*/
		ADC_DeInit(ADC1);
		ADC1_InitStructure.ADC_Mode = ADC_Mode_Independent;
		ADC1_InitStructure.ADC_ScanConvMode = ENABLE;
		ADC1_InitStructure.ADC_ContinuousConvMode = ENABLE;
		ADC1_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
		ADC1_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
		ADC1_InitStructure.ADC_NbrOfChannel = 3;
		ADC_Init(ADC1, &ADC1_InitStructure);
		
		    /* Select the channels to be read from **************************************/
    ADC_RegularChannelConfig(ADC1, ADC_CT1, 1, ADC_SampleTime_55Cycles5);//CT1
    ADC_RegularChannelConfig(ADC1, ADC_CT2, 2, ADC_SampleTime_55Cycles5);//CT2
    ADC_RegularChannelConfig(ADC1, ADC_VT1, 3, ADC_SampleTime_55Cycles5);//VT1

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


void getdata_time(uint16_t time)
	{
    uint16_t time_n=time;
		uint16_t uadc_2_raw =0 ;
		uint16_t iadc_1_raw =0 ;
		uint16_t iadc_2_raw =0 ;
		vtac.uadc[k] = 0;
		ct1.iadc[k]=0;
		ct2.iadc[k]=0;
	  while(time_n!=0)
		{
		 uadc_2_raw = read_adc(ADC1,ADC_VT1);
		 iadc_1_raw = read_adc(ADC1,ADC_CT1);
		 iadc_2_raw = read_adc(ADC1,ADC_CT2);
		 vtac.uadc[k] += uadc_2_raw;
		 ct1.iadc[k] += iadc_1_raw;
		 ct2.iadc[k] += iadc_2_raw;
		time_n--;
		}	
		vtac.uadc[k] /= time;
		ct1.iadc[k] /= time;
		ct2.iadc[k] /= time;
	}
	
	
	uint16_t	read_adc(ADC_TypeDef* ADCx,uint8_t ADC_Channel)
	  {
    uint16_t value_adc =0;
		value_adc =0;
		    
			//Delay_us(100);
			if(ADC_Channel == ADC_CT1)	value_adc =  ADC1ConvertedValue[0];
			if(ADC_Channel == ADC_CT2)  value_adc =  ADC1ConvertedValue[1];
			if(ADC_Channel == ADC_VT1)  value_adc =  ADC1ConvertedValue[2];
			return value_adc;
//			  result = value_adc;
//		    return result;
	}

float convert_adc(uint16_t value_adc, int scale)
	{
		float result =0 ;
		result =(value_adc-2048 )*scale/1240;
		return result;
		}

float RMS_cal(float	*value, int len_value)// caculator RMS
	{
		float result;
		result =0;
		int m;
		for(m=0;m < len_value ; m++)
		{
			result = result + (*(value+m))*(*(value+m));
			}
		result = result/len_value;// result = result / len_value
		result = sqrt(result);
	return result;
		}
float P_CAL(float *value1,float *value2, int len_value)//caculator  Potestas  (w)
{
	float result =0;
	int m;
	int phase_calib = 360;
	int _val1, _val2;
	for(m = 0; m < len_value; m++)
	{
		_val1 = (m + phase_calib) - ((m + phase_calib)/len_value)*len_value;
		//result = result + (*(value1 + m))*(*(value2 + m));
		result = result + (*(value1 + _val1))*(*(value2 + m));
		}
	result = result/len_value;// result = result / len_value
	//result = value1*value2*0.002;
	return result;
	}
float PF_cal(float u, float i, float p)//caculator POWER FACTOR
	{
		float result =0;
		result = p/(u*i);
    return result;
		}
//float f_cal()//caculator frenquence(Hz)
//{
//    float result;
//	  return result;
//}

	
	
	void measure_init(void)
		{
			ADC_Configuration();// CT1/CT2/VT1  
			TIM4_Configuration();
			vtac.Uavr = 0;
			vtac.favr = 0;
			ct1.Iavr = 0;
			ct2.Iavr = 0;
			p1.Pavr = 0;
			p1.PFavr = 0;
			p2.Pavr = 0;
			p2.PFavr = 0;
			}
	
	void measure_data_AC(void)
	{
			int point_1=0;
			int point_2=0;
			
			char diff ;   //diff= 1-> tang(point2>=point1)       ; diff = 0 -> giam;
			char done;
			// check zero crossing, voltage increase
			TIM_SetCounter(TIM4,0);
			k=0;

			do {
			getdata_time(15);
			point_1 = vtac.uadc[0];
			getdata_time(15);
			point_2 = vtac.uadc[0];
				}while((((point_2 - point_1)<2)&&(point_2 >= point_1))||(((point_2 - point_1)>-2)&&(point_2 < point_1)));
			if (point_2 >= point_1) diff = 1;
			else	diff = 0;
					
		

				done = 0;

					
					
			switch(diff)
				{
				case 1:// >= tang hoac bang
					{
						//turn on timer to measure frenquence
						TIM_SetCounter(TIM4,0);	
						//get data
							do
						{
									k++;
									getdata_time(15);
									if(((vtac.uadc[k-1] < point_1)&&(vtac.uadc[k] >= point_1))||(k == 499)){
											done = 1;
											vtac.time_f = (uint16_t)(TIM4->CNT);
									}
						}while(((uint16_t)(TIM4->CNT)<25000)&&((done==0)||(15000 > vtac.time_f))); 		//20ms = 40us*500
					break;
					}
				case 0:  // giam
					{
						//turn on timer to measure frenquence
						TIM_SetCounter(TIM4,0);
						//get data
							do
						{
									k++;
									getdata_time(15);
									if(((vtac.uadc[k-1] > point_1)&&(vtac.uadc[k] <= point_1))||(k == 499)){
											done = 1;
											vtac.time_f = (uint16_t)(TIM4->CNT);
									}
						}while(((uint16_t)(TIM4->CNT)<25000)&&((done==0)||(15000 > vtac.time_f))); 		//20ms = 40us*500
						
					break;
					}
				}
							
				
							
					
					//turn off timer to measure frenquence
					
					
					int m=0;
					//convert voltage adc -> current/voltage true
					for(m=0;m<k+1;m++)
					{
						ct1.i[m] = convert_adc(ct1.iadc[m],606);// => scale = calib*n*/R      :n: ti so CT: (100A/50mA); R = 3K3
					ct2.i[m] = convert_adc(ct2.iadc[m],606);    //mA
					vtac.u[m] = convert_adc(vtac.uadc[m],472);   //V;// scale = calib*n*(R1+R2)/R2     : n:ti so bien ap 220/10; R1 = 220k; R2 = 10k
						}
					//vtac.uadc[25]
						
					
					//caculator  U_RMS
					vtac.U= RMS_cal(vtac.u,k+1);
						
					// CHECK NOISE
					if(vtac.U <100) 
						{
								vtac.U = 0;
								for(m=0;m<k+1;m++)
								{
										ct1.i[m] = 0;// => scale = calib*n*/R      :n: ti so CT: (100A/50mA); R = 3K3
										ct2.i[m] = 0;    //mA
										vtac.u[m] = 0;   //0.1V;// scale = calib*n*(R1+R2)/R2     : n:ti so bien ap 220/10; R1 = 220k; R2 = 10k
								}
						}


					//caculator I_RMS, U_RMS, P, PF, frequence
					ct1.I= RMS_cal(ct1.i,k+1);
					ct2.I= RMS_cal(ct2.i,k+1);
					vtac.U= RMS_cal(vtac.u,k+1);
					if(((0 < vtac.time_f)&&(vtac.time_f < 25000))&&(vtac.U>100 ))vtac.f = 1000000/(float)vtac.time_f;
						else vtac.f = 0;
							
					p1.P = P_CAL(ct1.i,vtac.u,k+1);
					p2.P = P_CAL(ct2.i,vtac.u,k+1);
					p1.PF = PF_cal(vtac.U, ct1.I, p1.P);
					p2.PF = PF_cal(vtac.U, ct2.I, p2.P);
						r++;
						
					if(state ==1)//TRANS_STATE
					{
						ct1.I = ct1.Iavr;
						ct2.I = ct2.Iavr;
						vtac.U = vtac.Uavr;
						vtac.f = vtac.favr;
					  p1.P = p1.Pavr;
						p2.P = p2.Pavr;
						p1.PF =  p1.PFavr;
						p2.PF = p2.PFavr;
						}	
					
					if(vtac.f > 60){
						vtac.f = vtac.f;
						}
					
						
		}

		
void avr_data(void)
	{
				//if(timeout>30000) return;
				//ct1.Iavr = (ct1.Iavr*(r-1) + ct1.I)/r;
					ct1.Iavr = ct1.I;
				//ct2.Iavr = (ct2.Iavr*(r-1) + ct2.I)/r;
					ct2.Iavr = ct2.I;
				vtac.Uavr = (vtac.Uavr*(r-1) + vtac.U)/r;
				//vtac.Uavr = vtac.U;
				vtac.favr = (vtac.favr*(r-1) + vtac.f)/r;
				//vtac.favr = vtac.f;
				//p1.Pavr = (p1.Pavr*(r-1) + p1.P)/r;
					p1.Pavr = p1.P;
				//p2.Pavr = (p2.Pavr*(r-1) + p2.P)/r;
					p2.Pavr = p2.P;
				//p1.PFavr = (p1.PFavr*(r-1) + p1.PF)/r;
					p1.PFavr = p1.PF;
				//p2.PFavr = (p2.PFavr*(r-1) + p2.PF)/r;
					p2.PFavr = p2.PF;
				if(r>500) {
					r=0;
					//while(1){}
					}
		}
	//----------------------------------------------------------------------------------------------	

void send_data(void)
{
	//		printf("send data");
	printf("?node=solarpvtxac&fulljson={\"UAC_V\":%3.1f,\"I1_A\":%1.3f,\"I2_A\":%1.3f,\"P1_W\":%2.2f,\"P2_W\":%2.2f,\"f_Hz\":%2.2f,\"PF1\":%2.2f,\"PF2\":%2.2f}\n", (float) vtac.Uavr,(float) (ct1.Iavr/1000),(float) (ct2.Iavr/1000),(float) (p1.Pavr/1000),(float) (p2.Pavr/1000),(float) vtac.favr,(float) p1.PFavr,(float) p2.PFavr);
	r= 0;	
}
