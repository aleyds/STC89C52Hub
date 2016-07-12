#include "stdio.h"
#include "base_type.h"
#include <reg52.h>
#include "uart.h"
/*
unsigned long t1 = 0;
unsigned long t2 = 0;
sbit out = P2^0;
void main(void)
{
    EX0 = 1;//开外部中断0
    IT0 = 0;//中断触发方式

    TMOD = 0X22;//设置定时器工作方式T0,T1均为工作方式2（8位自动重装）  
    TH0 = 56;
    TL0 = 56;
    TH1 = 56;
    TL1 = 56;
    
    EA = 1; //开总中断
    while(1); 
}

void int0_int(void) interrupt 0
{
    EX0 = 0;//关外中断0
    ET0 = 1;//开定时器0中断
    TR0 = 1;//启动定时器0
    out = 0;//
}
void timer0_int(void) interrupt 1
{
    t1++;
    if( 5000 == t1 )		  //信号时间
    {   
    	t1 = 0;
        out = 1;
        TR0 = 0;//关定时器0
        ET0 = 0;//关定时器0中断
        ET1 = 1;//开定时器1中断
        TR1 = 1;//启动定时器1
        
    }
}
void timer1_int(void) interrupt 3
{
  
        t2++;
        if(80000 == t2)	    //不接收信号时间
        {    
             t2 = 0;
             TR1 = 0 ;
             ET1 = 0;
             EX0 = 1;//	开外部中断
        }
   
    
}
*/

sbit HightOut = P2^7;
//sbit TriggerIn = P2^7;

static unsigned int g_Timer0Counter = 0;
static unsigned long g_Timer1Counter = 0;
#define TIMER0_TIMEOUT		(20000) //2S  延时300ms  输出2S高电平
#define TIMER1_TIMEOUT		(120000) //15S
#define TIMERVALUE			(0xA4)  //定时器工作方式2  8位自动装载  16MHz  0x7B   12MHz 0x9C  11.0592MHz 0xA4

void _Delay(unsigned int ms)
{
	unsigned int i = 0;
	unsigned int j = 0;
	for(i = ms; i> 0; i--)
	{
		for(j = 110; j > 0; j--)
		{
			;//_nop_();
		}
	}
}

static void INIT1Start(void)
{
	EX1 = 1;//开外部中断0
    IT1 = 0;//中断触发方式
	
	
}

static void INIT1Stop(void)
{
	 EX1 = 0;//关外中断0
}

static void TimerInit(void)
{
	TMOD = 0x22;//设置定时器工作方式T0,T1均为工作方式2（8位自动重装）  
    TH0 = TIMERVALUE;//定时 100us
    TL0 = TIMERVALUE;
    TH1 = TIMERVALUE;
    TL1 = TIMERVALUE;
	g_Timer0Counter = 0;
	g_Timer1Counter = 0;
}

static void Timer0Start(void)
{
	ET0 = 1;//开定时器0中断
	TR0 = 1;//启动定时器0
}

static void Timer0Stop(void)
{
	TR0 = 0;//关定时器0
	ET0 = 0;//关定时器0中断
	g_Timer0Counter = 0;
}

static void Timer1Start(void)
{
	ET1 = 1;//开定时器1中断
	TR1 = 1;//启动定时器1
}

static void Timer1Stop(void)
{
	TR1 = 0 ;
	ET1 = 0;
	g_Timer1Counter = 0;
}

void main(void)
{
	HightOut = 0; 
	INIT1Start();
	//_UartOpen();
	
	/* HightOut = 0;
	_Delay(1000);
	HightOut = 1;
	_Delay(1000); */
	TimerInit();
	EA = 1; //开总中断
	//hs_printf(" Main Open !\n\r");
	while(1);
}

//外部中断1
static void INIT1IRQHandler() interrupt 2
{
	//if(HightOut) //如果正在输出高电平，则中断
	//{
		//Timer0Stop();
		//Timer1Stop();
		//INIT1Start();
		//HightOut = 1;
	//}else
	{
		_Delay(100);
		INIT1Stop();
		Timer0Start();//开启Time0定时输出电平
		HightOut = 1; // 输出高电平
	}
	
}

//定时器T0 中断响应
static void __Timer0IRQHandler(void) interrupt 1
{
	//hs_printf(" Timer0 IRQ  \n\r");
	g_Timer0Counter++;
	if(g_Timer0Counter >= TIMER0_TIMEOUT)
	{
		g_Timer0Counter = 0;
		Timer0Stop();
		Timer1Start();
		HightOut = 0;
	}
}

//定期器T1 中断响应
static void __Timer1IRQHandler(void) interrupt 3
{
	g_Timer1Counter++;
	if(g_Timer1Counter >= TIMER1_TIMEOUT)
	{
		g_Timer1Counter = 0;
		Timer1Stop();
		HightOut = 0;
		INIT1Start();
	}
}