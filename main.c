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

//led灯gpio初始化
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


 
 //pwm初始化
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
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE );
		
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);  
 
	//开启自动重装,允许arr在定时器开启时更改
	TIM_ARRPreloadConfig(TIM2,ENABLE);
//可以选择有中断或无中断

	TIM_Cmd(TIM2,ENABLE);
}
 

//可以选择有中断或无中断
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
				//修改预分频值
				//TIM_PrescalerConfig(TIM2,periods[i],TIM_PSCReloadMode_Immediate);
				
				//修改arr值,自动重装arr
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
				//修改预分频值
				//TIM_PrescalerConfig(TIM2,periods[i],TIM_PSCReloadMode_Immediate);
				
				//修改arr值,自动重装arr
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
				//修改arr值,自动重装arr stop motor
				TIM_SetAutoreload(TIM2, periods[1999-i]);
				TIM_SetCounter(TIM2,0);
				TIM_SetCompare1(TIM2, periods[1999-i]/2);
				i++;
				if(i>=2000)i=0;
			}
			else
			{
				//修改预分频值
				//TIM_PrescalerConfig(TIM2,periods[i],TIM_PSCReloadMode_Immediate);
				
				//修改arr值,自动重装arr
				TIM_SetAutoreload(TIM2, periods[i]);
				TIM_SetCounter(TIM2,0);
				TIM_SetCompare1(TIM2, periods[i]/2);
				i++;
			}
		//}
		//acc++;
		
	}


}


//其中的Fcurrent为length(1000)个点中的单个频率值。Fmin起始频率为5600; Fmax为最大频率64000;
//-flexible*(i - num)/num是对S型曲线进行拉伸变化，其中flexible代表S曲线区间（越大代表压缩的
//最厉害，中间（x坐标0点周围）加速度越大；越小越接近匀加速。理想的S曲线的取值为4-6），i是在循
//环计算过程中的索引，从0开始，num为 length/2 大小(这样可以使得S曲线对称)。在项目中i的
//区间[0,1000), num=1000/2=500。这些参数均可以修改。提供的计算接口如下。

/*  calculate the Period and Freq array value, fill the Period value into the Period register during the timer interrupt.
*calculate the acceleration procedure , a totally 1000 elements array.
* parameter    fre[]: point to the array that keeps the freq value.
*   period[]: point to the array that keeps the timer period value.
*   len: 此段加速所分的点的个数 the procedure of acceleration length.it is best thing to set the float number, some compile software maybe transfer error if set it as a int
*   fre_max: 最大速度，频率值 单位HZ maximum speed, frequency vale.
*   fre_min: 最小速度，频率值 单位HZ start minimum speed, frequency vale.  mind : 10000000/65535 = 152, so fre_min can't less than 152.
*   flexible:  flexible value. adjust the S curvesflexible 代表S曲线区间（越大代表压缩的最厉害，中间（x坐标0点周围）加速度越大；越小越接近匀加速。理想的S曲线的取值为4-6）
*/
//S曲线加速
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
				period[i] = (unsigned short)(72000000.0 / fre[i]);  //这里我的分频是1，所以定时器频率是72M 
        //period[i] = (unsigned short)(10000000.0 / fre[i]);    //这里的10M是定时器的频率 10000000 is the timer driver frequency
    
			//这里需要注意arr重装值是不能大于65535的，所以限制一下,频率得大于这个等式的结果 定时器频率/65535 = 频率
			//例子：   72M/65535 = 1098.64 HZ    输入最小频率要大于1098.64
			
			
		}


    return ;
}













//频率 5k,10k,20k,50k,100k
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

