#include "ultrasonic.h"
#include "delay.h"
#include "usart.h"

#define N 13  //��λֵ�˲�����
u8 i=0;
u32 TIME;	//���벶��ֵ	 ��¼TIM->CNT��ֵ
/********************************************************************************
����ʹ�õ��Ƕ�ʱ��2���в���
�Զ���װֵΪ0	 ��Ƶ��Ϊ  72	
T=(arr*(psc+1))/(36M*2)
�˴�72��Ƶ������Ƶ��Ϊ1M
Tx������>PA.1     Rx��������>PA.0
********************************************************************************/
TIM_ICInitTypeDef  TIM2_ICInitStructure;

void ultrasonic_Init()
{	 
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	//x=TIMx
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //x=GPIOx
//------------------------RX,ECHO----------------------------------	
	GPIO_InitStructure.GPIO_Pin  = Rx_GPIO;  		//RX��ECHO
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;  //��������  
	GPIO_Init(ULT_GPIO, &GPIO_InitStructure);
	GPIO_ResetBits(ULT_GPIO,Rx_GPIO);		 
//------------------------TX,TRIG----------------------------------
	GPIO_InitStructure.GPIO_Pin  = Tx_GPIO;         //TX,Trig      
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;   
	GPIO_Init(ULT_GPIO, &GPIO_InitStructure);
//---------------------------TIMx���벶�����--------------------------------------	
	TIM_TimeBaseStructure.TIM_Period = 0XFFFF;//�Զ���װֵ 
	TIM_TimeBaseStructure.TIM_Prescaler =71; 	//Ԥ��Ƶ,T=(arr*(psc+1))/(36M*2)   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //���ϼ���
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);	 //x=TIMx
//-----------------------------TIMx���벶�����------------------------------------------- 
//PA0--TIM2_CH1	 PA1--TIM2_CH2  PA2--TIM2_CH3  PA3--TIM2_CH4
	TIM2_ICInitStructure.TIM_Channel = TIM_Channel_1; //ѡ��ͨ��,TIM_Channel_x=TIMx_CHx
	TIM2_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
	TIM2_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
	TIM2_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //���������Ƶ,����Ƶ 
	TIM2_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 ���������˲��� ���˲�
	TIM_ICInit(TIM2, &TIM2_ICInitStructure);  //x=TIMx
//---------------------------------------NVIC��ʼ��------------------------------------------ 
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn; //TIMx_IRQn=TIMx
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; //��ռ���ȼ�2��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; //�����ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ��� 
	
	TIM_ITConfig(TIM2,TIM_IT_Update|TIM_IT_CC1,ENABLE);//��������ж� ,����CCxIE�����ж�,CHx=TIM_IT_CCx	
	
   	TIM_Cmd(TIM2,ENABLE ); 	//ʹ��,x=TIMx  
}    	 
////��ʱ��5�жϷ������	 
void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET)//��������,x=TIMx,TIM_IT_CCx= TIM_CHx
		if(i==0)
		{
			 TIM_SetCounter(TIM2,0);	 //��������,x=TIMx
			 TIM_OC1PolarityConfig(TIM2,TIM_ICPolarity_Falling); //�½��ش���,x=TIMx,CHx=TIM_OCxPolarityConfig
			 i=1;				//�����ص���
		}
		else
		{
			TIME=TIM_GetCounter(TIM2);	//��ò���ֵ,x=TIMx ���ߵ�ƽʱ��
			TIM_OC1PolarityConfig(TIM2,TIM_ICPolarity_Rising);//�Ͻ��ش���,x=TIMx,CHx=TIM_OCxPolarityConfig
			i=0;				//�½��ص��� 			
		}
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC1|TIM_IT_Update);//����жϱ�־��x=TIMx,TIM_IT_CCx= TIM_CHx
}



float MiddleValueFilter() //��ֵ�˲�
{ 
  u8 i,j,k; 
  float temp; 	   //�����м����
  float ArrDataBuffer[N]; 
  for(i=0; i<N; i++) //һ�βɼ�N�����ݷ��������� 
  { 
    ArrDataBuffer[i]=get_distance(TIME); 
    delay_ms(1); 
  } 
  for(j=0; j<=N-1; j++) //����ֵ��С�������� 
  { 
    for(k=0; k<=N-j-1; k++) 
    { 
      if(ArrDataBuffer[k]>ArrDataBuffer[k+1]) 
      { 
        temp=ArrDataBuffer[k]; 
        ArrDataBuffer[k]=ArrDataBuffer[k+1]; 
        ArrDataBuffer[k+1]=temp; 
      } 
    } 
  } 
  return(ArrDataBuffer[(N-1)/2]);//ȡ�м�ֵ 
} 

float get_distance(u32 TIME)	
{
		float distance; 
		GPIO_SetBits(ULT_GPIO,Tx_GPIO);//��>10US�ĸߵ�ƽ
		delay_ms(1);
		GPIO_ResetBits(ULT_GPIO,Tx_GPIO);	//��������
		distance=(float)TIME*340/20000;	//�õ�����
		return distance;	 
}
