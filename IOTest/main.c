#include "stdio.h"
#include <reg52.h>
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

sbit HightOut = P2^0;
sbit TriggerIn = P2^1;

static unsigned int g_Timer0Counter = 0;
static unsigned int g_Timer1Counter = 0;
#define TIMER0_TIMEOUT		(3000) //300mS  延时300ms  输出3S高电平
#define TIMER1_TIMEOUT		(30000) //3S

static void INIT0Start(void)
{
	EX0 = 1;//开外部中断0
    IT0 = 0;//中断触发方式
}

static void INIT0Stop(void)
{
	 EX0 = 0;//关外中断0
}

static void TimerInit(void)
{
	TMOD = 0X22;//设置定时器工作方式T0,T1均为工作方式2（8位自动重装）  
    TH0 = 0x7B;//16MHz 晶振  定时 100us
    TL0 = 0x7B;
    TH1 = 0x7B;
    TL1 = 0x7B;
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
	INIT0Start();
	TimerInit();
	EA = 1; //开总中断
	while(1);
}

//外部中断0
static void INIT0IRQHandler() interrupt 0
{
	if(HightOut) //如果正在输出高电平，则中断
	{
		Timer0Stop();
		Timer1Stop();
		INIT0Start();
		HightOut = 0;
	}else
	{
		INIT0Stop();
		Timer0Start();
		HightOut = 0;
	}
	
}

//定时器T0 中断响应
static void __Timer0IRQHandler(void) interrupt 1
{
	g_Timer0Counter++;
	if(g_Timer0Counter >= TIMER0_TIMEOUT)
	{
		g_Timer0Counter = 0;
		Timer0Stop();
		Timer1Start();
		HightOut = 1;
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
		INIT0Start();
	}
}