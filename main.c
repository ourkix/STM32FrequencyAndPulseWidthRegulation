#define USE_STM3210B_EVAL

#include "stm32f10x.h"
#include "math.h"


#define uint32 unsigned int

volatile float fress[1000] = {0};
volatile unsigned short periods[1000] = {0};

volatile uint32 pulse_width = 0;
volatile uint32	direction = 0;
 
void delay_ms(uint16_t ms)
{
	uint32_t i, j;
	j = ms * 19200; // j = ms / 1000.0 * 48000000 / 2.5;  
	for (i=0; i<j; i++) {
	}
} 

//led��gpio��ʼ��
void ledinit()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB , ENABLE); 						 					 
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB , GPIO_Pin_12);
	
}

void ledflash()
{
	static char ledstate = 0;

	ledstate = ~ledstate;
	if(ledstate > 0)
	{
		GPIO_SetBits(GPIOB , GPIO_Pin_12);
	}
	else
	{
		GPIO_ResetBits(GPIOB , GPIO_Pin_12);
	}

}


 
 //pwm��ʼ��
void PWM_Init(u16 arr,u16 psc)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA,ENABLE);
 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;        
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                    //IO�ڸ����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                  //IO���ٶ�
	GPIO_Init(GPIOA, &GPIO_InitStructure);	//USART���IO��
	
	TIM_DeInit(TIM2);
	
	
	TIM_TimeBaseStructure.TIM_Period = arr; //��ʱ����
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //Ԥ��Ƶ1��36M
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //ʱ�ӷ�Ƶ����
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //���PWMģʽ
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //ʹ�����
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//�������
	TIM_OCInitStructure.TIM_Pulse = 50;
	
	TIM_OC1Init(TIM2,&TIM_OCInitStructure);
	TIM_OC2Init(TIM2,&TIM_OCInitStructure);
	TIM_OC3Init(TIM2,&TIM_OCInitStructure);
	TIM_OC4Init(TIM2,&TIM_OCInitStructure);
	
	TIM_OC1PreloadConfig(TIM2,TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM2,TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM2,TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM2,TIM_OCPreload_Enable);
 
//����ѡ�����жϻ����ж�
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE );
		
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);  
 
	//�����Զ���װ,����arr�ڶ�ʱ������ʱ����
	TIM_ARRPreloadConfig(TIM2,ENABLE);
//����ѡ�����жϻ����ж�

	TIM_Cmd(TIM2,ENABLE);
}
 

//����ѡ�����жϻ����ж�
void TIM2_IRQHandler(void)
{
//	if(TIM_GetITStatus(TIM2,TIM_IT_Update) != RESET)
//	{
//		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
//		
//		if(pulse_width == 0)
//			direction = 0;
//		else if(pulse_width == 100)
//			direction = 1;
// 
//		if(direction == 0)
//			pulse_width++;
//		else
//			pulse_width--;
//		TIM_SetCompare1(TIM2, pulse_width);
//		
//	}
	
	static int i = 0;

	if(TIM_GetITStatus(TIM2,TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
		
		

			
			if(i >= 1000){

			}
			else
			{
				//�޸�Ԥ��Ƶֵ
				//TIM_PrescalerConfig(TIM2,periods[i],TIM_PSCReloadMode_Immediate);
				
				//�޸�arrֵ,�Զ���װarr
				TIM_SetAutoreload(TIM2, periods[i]);
				TIM_SetCounter(TIM2,0);
				TIM_SetCompare1(TIM2, periods[i]/2);
				i++;
			}
		
		
		
	}
	
 
}
 

void stepmotoraccspeedup()
{
	static int i = 0;
	static int acc = 0;
	
	if(TIM_GetITStatus(TIM2,TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
		//if(acc > 72000)
		//{
		//	acc = 0;
			
			if(i >= 1000){

			}
			else
			{
				//�޸�Ԥ��Ƶֵ
				//TIM_PrescalerConfig(TIM2,periods[i],TIM_PSCReloadMode_Immediate);
				
				//�޸�arrֵ,�Զ���װarr
				TIM_SetAutoreload(TIM2, periods[i]);
				TIM_SetCounter(TIM2,0);
				TIM_SetCompare1(TIM2, periods[i]/2);
				i++;
			}
		//}
		//acc++;
		
	}

}


void stepmotoraccspeedupdown()
{
	
	static int i = 0;
	static int acc = 0;
	
	if(TIM_GetITStatus(TIM2,TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
		//if(acc > 72000)
		//{
		//	acc = 0;
			
			if(i >= 1000){
				//�޸�arrֵ,�Զ���װarr stop motor
				TIM_SetAutoreload(TIM2, periods[1999-i]);
				TIM_SetCounter(TIM2,0);
				TIM_SetCompare1(TIM2, periods[1999-i]/2);
				i++;
				if(i>=2000)i=0;
			}
			else
			{
				//�޸�Ԥ��Ƶֵ
				//TIM_PrescalerConfig(TIM2,periods[i],TIM_PSCReloadMode_Immediate);
				
				//�޸�arrֵ,�Զ���װarr
				TIM_SetAutoreload(TIM2, periods[i]);
				TIM_SetCounter(TIM2,0);
				TIM_SetCompare1(TIM2, periods[i]/2);
				i++;
			}
		//}
		//acc++;
		
	}


}


//���е�FcurrentΪlength(1000)�����еĵ���Ƶ��ֵ��Fmin��ʼƵ��Ϊ5600; FmaxΪ���Ƶ��64000;
//-flexible*(i - num)/num�Ƕ�S�����߽�������仯������flexible����S�������䣨Խ�����ѹ����
//���������м䣨x����0����Χ�����ٶ�Խ��ԽСԽ�ӽ��ȼ��١������S���ߵ�ȡֵΪ4-6����i����ѭ
//����������е���������0��ʼ��numΪ length/2 ��С(��������ʹ��S���߶Գ�)������Ŀ��i��
//����[0,1000), num=1000/2=500����Щ�����������޸ġ��ṩ�ļ���ӿ����¡�

/*  calculate the Period and Freq array value, fill the Period value into the Period register during the timer interrupt.
*calculate the acceleration procedure , a totally 1000 elements array.
* parameter    fre[]: point to the array that keeps the freq value.
*   period[]: point to the array that keeps the timer period value.
*   len: �˶μ������ֵĵ�ĸ��� the procedure of acceleration length.it is best thing to set the float number, some compile software maybe transfer error if set it as a int
*   fre_max: ����ٶȣ�Ƶ��ֵ ��λHZ maximum speed, frequency vale.
*   fre_min: ��С�ٶȣ�Ƶ��ֵ ��λHZ start minimum speed, frequency vale.  mind : 10000000/65535 = 152, so fre_min can't less than 152.
*   flexible:  flexible value. adjust the S curvesflexible ����S�������䣨Խ�����ѹ�������������м䣨x����0����Χ�����ٶ�Խ��ԽСԽ�ӽ��ȼ��١������S���ߵ�ȡֵΪ4-6��
*/
//S���߼���
void CalculateSModelLine(float * fre,  unsigned short * period,   float  len,  float fre_max,  float fre_min, float flexible)
{
    int i=0;
    float deno ;
    float melo ;
    float delt = fre_max-fre_min;
    for(; i<len; i++)
    {
        melo = flexible * (i-len/2) / (len/2);
        deno = 1.0 / (1 + expf(-melo));   //expf is a library function of exponential(e) 
        fre[i] = delt * deno + fre_min;
				period[i] = (unsigned short)(72000000.0 / fre[i]);  //�����ҵķ�Ƶ��1�����Զ�ʱ��Ƶ����72M 
        //period[i] = (unsigned short)(10000000.0 / fre[i]);    //�����10M�Ƕ�ʱ����Ƶ�� 10000000 is the timer driver frequency
    
			//������Ҫע��arr��װֵ�ǲ��ܴ���65535�ģ���������һ��,Ƶ�ʵô��������ʽ�Ľ�� ��ʱ��Ƶ��/65535 = Ƶ��
			//���ӣ�   72M/65535 = 1098.64 HZ    ������СƵ��Ҫ����1098.64
			
			
		}


    return ;
}













//Ƶ�� 5k,10k,20k,50k,100k
unsigned short fres[5] = {14399,7199,3599,1439,719};
int i; 

void mainsource()
{
	
		ledinit();
 
		PWM_Init(99,7199);
 
		while(1)
		{
 
		for(i=0;i<5;i++)
		{
        //����� tim_cmd disable �� enable ���Բ��ӣ�ûӰ��  delay_msҲ��
   		TIM_Cmd(TIM2,DISABLE);
        delay_ms(5);
		TIM_PrescalerConfig(TIM2,fres[i],TIM_PSCReloadMode_Immediate);
    TIM_SetCounter(TIM2,0);
		TIM_Cmd(TIM2,ENABLE);
		delay_ms(50);
		}
			
		}
	
}




void mainSmodel()
{
	

	
		ledinit();
 
	  CalculateSModelLine( (float *)fress,(unsigned short *)periods,  1000, 50000,  2000, 4);
	
		PWM_Init(2000,0);
 
	  
		
		while(1)
		{
 
			ledflash();
			delay_ms(10);
			
		}
	
}





int main()                                                                          
{   
 //mainsource();
	mainSmodel();
}

