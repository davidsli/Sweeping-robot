#include "sys.h"
#include "led.h"
#include "motor.h"
#include "delay.h"
#include "hc05.h"
#include "usart2.h"			 	 
#include "string.h"	
#include "usart.h"
#include "ultrasonic.h"
extern u32	TIME;	//���벶��ֵ	


int main(void)
{
	  float temp;	 //����ֵ
	  int distance=0, flag=0;
	  u8 reclen=0, sendcnt=0; 
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	  uart_init(9600);
	  LED_Init();
	  delay_init();
	  TIM3_PWM_Init(899,0);
	  HC05_Init();
	  ultrasonic_Init();               //�Գ�����ģ���ʼ��
    while(1)
		{	
			
			if(USART2_RX_STA&0X8000)			//���յ�һ��������
			{		
				reclen=USART2_RX_STA&0X7FFF;	//�õ����ݳ���
				USART2_RX_BUF[reclen]=0;	 	//���������		
				
				if(strcmp((const char*)USART2_RX_BUF,"F")==0)
				{
					run_init();//ǰ��
					flag=1;
				}
				
				
				if(strcmp((const char*)USART2_RX_BUF,"B")==0)
				{
					back_init();//����
					flag=1;
				}
				
				if(strcmp((const char*)USART2_RX_BUF,"L")==0)
				{
					left_init();//��ת
					flag=1;
				}
				
				if(strcmp((const char*)USART2_RX_BUF,"R")==0)
				{
					right_init();//��ת
					flag=1;
				}
				
				if(strcmp((const char*)USART2_RX_BUF,"S")==0)
				{
					stod_init();//ֹͣ
					flag=0;
				}
				
				USART2_RX_STA=0;
				
			}
			temp=MiddleValueFilter(); //��ֵ�˲�
			distance=(int)temp;
			sendcnt = (u8)temp;
			u2_printf("%d cm\r\n",sendcnt);
			if((distance <= 10)&&flag)
			{
				stod_init();
				delay_ms(1000);
				left_init();
				delay_ms(500);
				run_init();
			}
			LED=!LED;
			delay_ms(100);
		} 
}
