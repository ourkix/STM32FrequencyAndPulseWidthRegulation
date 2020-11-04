# STM32FrequencyAndPulseWidthRegulation
stm32f103c8t6 脉宽和频率调节示例

TIM_Period  溢出计数值，（如有中断）达到这个值就中断  arr

TIM_Prescaler 预分频 psc

调节占空比 TIMx_SetCompare(1,0-arr);         //TIM x为定时器数字如TIM2、TIM3，参数1位通道，参数2为ccr 此致时间内与电平翻转 比如默认0-arr都为高电平，如setcompare的值为arr/2，就是0-arr/2 为低电平  arr/2-arr为高电平  占空比 50%。


时钟频率为 72M

公式：

时钟频率(system clk) / 预分频(psc) /想要的pwm频率 = arr（重装值）

例子 ： 72M / 1 / 2000HZ  = 36000

arr和psc在设置的时候都要减一，因为在硬件上会自动加一，就是 psc = 0 ， arr = 35999

 

TIM_TimeBaseStructure.TIM_Period = (1000-1); //设置在下一个更新事件装入 活动活动的自动重装载寄存器周期的值计数到1000为1ms

TIM_TimeBaseStructure.TIM_Prescaler =(72-1); //设置用来作为TIMx时钟频率除数的预分频值  1M的计数频率

                     arr   * psc  /  时钟频率 = 定时时间

定时时间T = 1000 * 72 / 72000 000 = 1ms

 

TIM_Prescaler = 72 – 1; t = 72 / 72000 000 = 1us,即TIMx->CNT每1us加一次

TIM_Period = 1000;当TIMx->CNT计数值达到1000us(也就是一开始计算的定时时间1ms),进入中断服务函数,msHcCount将自动加一
