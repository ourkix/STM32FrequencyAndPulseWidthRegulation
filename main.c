#include "stm32f10x.h"

#define uint32 unsigned int



volatile uint32 pulse_width = 0;
volatile uint32	direction = 0;
 
void delay_ms(uint16_t ms)
{
	uint32_t i, j;
	j = ms * 19200; // j = ms / 1000.0 * 48000000 / 2.5;  
	for (i=0; i<j; i++) {
	}
} 


//1.通过更改预分频器的值，改变计数器的频率的方式改变PWM波的频率
//在STM32F103中有封装好的调节预分频器的函数 TIM_PrescalerConfig()；直接调用即可。
//溢出计数值(arr  值域0-65536)=72MHz/想得到的频率
//按照72M的预分频，频率 （范围约等于）= 1100HZ  - 72MHZ
//例子：
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

 
 
void PWM_Init(u16 arr,u16 psc)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA,ENABLE);
 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;        
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                    //IO口复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                  //IO口速度
	GPIO_Init(GPIOA, &GPIO_InitStructure);	//USART输出IO口
	
	TIM_DeInit(TIM2);
	
	TIM_TimeBaseStructure.TIM_Period = arr; //定时周期
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //预分频1，36M
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //时钟分频因子
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //输出PWM模式
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //使能输出
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//输出极性
	TIM_OCInitStructure.TIM_Pulse = 50;
	
	TIM_OC1Init(TIM2,&TIM_OCInitStructure);
	TIM_OC2Init(TIM2,&TIM_OCInitStructure);
	TIM_OC3Init(TIM2,&TIM_OCInitStructure);
	TIM_OC4Init(TIM2,&TIM_OCInitStructure);
	
	TIM_OC1PreloadConfig(TIM2,TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM2,TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM2,TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM2,TIM_OCPreload_Enable);
 
//可以选择有中断或无中断
//	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE );
//		
//	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn; 
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);  
 
 
	//TIM_ARRPreloadConfig(TIM2,ENABLE);
	TIM_Cmd(TIM2,ENABLE);
}
 
//可以选择有中断或无中断
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
		
		if(pulse_width == 0)
			direction = 0;
		else if(pulse_width == 100)
			direction = 1;
 
		if(direction == 0)
			pulse_width++;
		else
			pulse_width--;
		TIM_SetCompare1(TIM2, pulse_width);
		
	}
 
}
 
//频率 5k,10k,20k,50k,100k
unsigned short fres[5] = {14399,7199,3599,1439,719};
int i; 

int main()                                                                          
{   
 
		ledinit();
 
		PWM_Init(99,7199);
 
		while(1)
		{
 
		for(i=0;i<5;i++)
		{
        //这里的 tim_cmd disable 和 enable 可以不加，没影响  delay_ms也是
   		TIM_Cmd(TIM2,DISABLE);
        delay_ms(5);
		TIM_PrescalerConfig(TIM2,fres[i],TIM_PSCReloadMode_Immediate);
        TIM_SetCounter(TIM2,0);
		TIM_Cmd(TIM2,ENABLE);
		delay_ms(50);
		}
			
		}
}



////2通过普通定时器的中断去更新gpio的输出方式改变PWM波的频率，微调setcount 的值可以平缓过渡频率
////例子：

//void ledinit()
//{
//	GPIO_InitTypeDef GPIO_InitStructure;
//	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB , ENABLE); 						 					 
//	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
//	GPIO_SetBits(GPIOB , GPIO_Pin_12);
//	
//}
// 
//void MY_TIM3_Init(u16 arr,u16 psc){
// 
//	NVIC_InitTypeDef NVIC_InitStructure;
//    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
// 
// 
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
// 
// 
//    TIM_TimeBaseStructure.TIM_Period = arr;
//    TIM_TimeBaseStructure.TIM_Prescaler = psc;
// 
// 
//    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
//    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //????
//    TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);
//		
//	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE );
//	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; 
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  
//	NVIC_Init(&NVIC_InitStructure);  
// 
//    TIM_Cmd(TIM3,ENABLE);
//}
// 
//void TIM3_IRQHandler(void)
//{
//	if(TIM_GetITStatus(TIM3,TIM_IT_Update) != RESET)
//	{
//		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
//		ledcount++;
//		if(ledcount > setcount)
//		{
//			ledcount = 0;
//			ledstate = ~ledstate;
//			if(ledstate > 0)
//			{
//				GPIO_SetBits(GPIOB , GPIO_Pin_12);
//			}
//			else
//			{
//				GPIO_ResetBits(GPIOB , GPIO_Pin_12);
//			}
//			
//			
//		}
// 
//		
//	}
//}
// 
//volatile uint32 ledcount = 0;
//volatile uint32 setcount = 1;
// 
//int main()                                                                          
//{  
//    MY_TIM3_Init(9,7199);
//    ledinit();
// 
// 
//		while(1)
//		{
//			setcount++;
//			if(setcount > 100)
//			{
//					setcount = 1;
//			}
//			delay_ms(100);
//        }
//}













////3通过更改arr值改变PWM波的频率
////例子：

//void PWM_Init(u16 arr,u16 psc)
//{
//	GPIO_InitTypeDef  GPIO_InitStructure;
//	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
//	TIM_OCInitTypeDef TIM_OCInitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
//	
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA,ENABLE);
// 
//	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;        
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                    //IO口复用推挽输出
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                  //IO口速度
//	GPIO_Init(GPIOA, &GPIO_InitStructure);	//USART输出IO口
//	
//	TIM_DeInit(TIM2);
//	
//	TIM_TimeBaseStructure.TIM_Period = arr; //定时周期
//	TIM_TimeBaseStructure.TIM_Prescaler =psc; //预分频1，36M
//	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //时钟分频因子
//	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数模式
//	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
//	
//	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //输出PWM模式
//	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //使能输出
//	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//输出极性
//	TIM_OCInitStructure.TIM_Pulse = (int)(arr+1)/2;;
//	
//	
// 
//	
//	TIM_OC1Init(TIM2,&TIM_OCInitStructure);
////	TIM_OC2Init(TIM2,&TIM_OCInitStructure);
////	TIM_OC3Init(TIM2,&TIM_OCInitStructure);
////	TIM_OC4Init(TIM2,&TIM_OCInitStructure);
//	
//	TIM_OC1PreloadConfig(TIM2,TIM_OCPreload_Enable);
////	TIM_OC2PreloadConfig(TIM2,TIM_OCPreload_Enable);
////	TIM_OC3PreloadConfig(TIM2,TIM_OCPreload_Enable);
////	TIM_OC4PreloadConfig(TIM2,TIM_OCPreload_Enable);
// 
// 
////	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE );
////		
////	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn; 
////	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 
////	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  
////	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
////	NVIC_Init(&NVIC_InitStructure);  
// 
// 
//	TIM_ARRPreloadConfig(TIM2,ENABLE);
//	TIM_Cmd(TIM2,ENABLE);
//}
// 
// 
// 
////PWM_Init(35999,0);				//2k, 4k,   8,    16k,   32k,     64k    144k   480k
//unsigned short sfresarr[8] = {35999,17999,8999, 4499,  2249   ,  1124   ,499   ,149 };
// 
//int main()
//{
//	SystemInit();
//    PWM_Init(35999,0);
//	while(1)
//	{
//		TIM_Cmd(TIM2,DISABLE);
//		TIM_ARRPreloadConfig(TIM2,DISABLE);
//		delay_ms(20);
//		TIM_SetCompare1(TIM2, sfresarr[i+1]);
//		TIM_SetAutoreload(TIM2, sfresarr[7]);
//		TIM_ARRPreloadConfig(TIM2,ENABLE);
//        TIM_SetCounter(TIM2,0);
//		TIM_Cmd(TIM2,ENABLE);
//		delay_ms(500);
//    }
//}





