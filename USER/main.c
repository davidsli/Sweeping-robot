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
	  int distance=0;
	  u8 reclen=0; 
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	  uart_init(9600);
	  LED_Init();
	  delay_init();
	  printf("ok1\n");
	  TIM3_PWM_Init(899,0);
	  printf("ok3\n");
	  HC05_Init();
	  printf("ok4\n");
	  ultrasonic_Init();               //�Գ�����ģ���ʼ��
	  printf("ok5\n");
    while(1)
		{	
			
			if(USART2_RX_STA&0X8000)			//���յ�һ��������
			{		
				reclen=USART2_RX_STA&0X7FFF;	//�õ����ݳ���
				USART2_RX_BUF[reclen]=0;	 	//���������		
				
				if(strcmp((const char*)USART2_RX_BUF,"F")==0)run_init();//ǰ��
				
				
				if(strcmp((const char*)USART2_RX_BUF,"B")==0)back_init();//����
				
				if(strcmp((const char*)USART2_RX_BUF,"L")==0)left_init();//��ת
				
				if(strcmp((const char*)USART2_RX_BUF,"R")==0)right_init();//��ת
				
				if(strcmp((const char*)USART2_RX_BUF,"S")==0)stod_init();//ֹͣ
				
				USART2_RX_STA=0;
				
			}
			temp=MiddleValueFilter(); //��ֵ�˲�
			distance=(int)temp;
			LED=!LED;
			printf("distance %fcm\n",temp);
			printf("distance %dcm\n",distance);
			delay_ms(100);
		} 
}
