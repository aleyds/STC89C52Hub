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
    EX0 = 1;//���ⲿ�ж�0
    IT0 = 0;//�жϴ�����ʽ

    TMOD = 0X22;//���ö�ʱ��������ʽT0,T1��Ϊ������ʽ2��8λ�Զ���װ��  
    TH0 = 56;
    TL0 = 56;
    TH1 = 56;
    TL1 = 56;
    
    EA = 1; //�����ж�
    while(1); 
}

void int0_int(void) interrupt 0
{
    EX0 = 0;//�����ж�0
    ET0 = 1;//����ʱ��0�ж�
    TR0 = 1;//������ʱ��0
    out = 0;//
}
void timer0_int(void) interrupt 1
{
    t1++;
    if( 5000 == t1 )		  //�ź�ʱ��
    {   
    	t1 = 0;
        out = 1;
        TR0 = 0;//�ض�ʱ��0
        ET0 = 0;//�ض�ʱ��0�ж�
        ET1 = 1;//����ʱ��1�ж�
        TR1 = 1;//������ʱ��1
        
    }
}
void timer1_int(void) interrupt 3
{
  
        t2++;
        if(80000 == t2)	    //�������ź�ʱ��
        {    
             t2 = 0;
             TR1 = 0 ;
             ET1 = 0;
             EX0 = 1;//	���ⲿ�ж�
        }
   
    
}
*/

sbit HightOut = P2^7;
//sbit TriggerIn = P2^7;

static unsigned int g_Timer0Counter = 0;
static unsigned long g_Timer1Counter = 0;
#define TIMER0_TIMEOUT		(20000) //2S  ��ʱ300ms  ���2S�ߵ�ƽ
#define TIMER1_TIMEOUT		(120000) //15S
#define TIMERVALUE			(0xA4)  //��ʱ��������ʽ2  8λ�Զ�װ��  16MHz  0x7B   12MHz 0x9C  11.0592MHz 0xA4

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
	EX1 = 1;//���ⲿ�ж�0
    IT1 = 0;//�жϴ�����ʽ
	
	
}

static void INIT1Stop(void)
{
	 EX1 = 0;//�����ж�0
}

static void TimerInit(void)
{
	TMOD = 0x22;//���ö�ʱ��������ʽT0,T1��Ϊ������ʽ2��8λ�Զ���װ��  
    TH0 = TIMERVALUE;//��ʱ 100us
    TL0 = TIMERVALUE;
    TH1 = TIMERVALUE;
    TL1 = TIMERVALUE;
	g_Timer0Counter = 0;
	g_Timer1Counter = 0;
}

static void Timer0Start(void)
{
	ET0 = 1;//����ʱ��0�ж�
	TR0 = 1;//������ʱ��0
}

static void Timer0Stop(void)
{
	TR0 = 0;//�ض�ʱ��0
	ET0 = 0;//�ض�ʱ��0�ж�
	g_Timer0Counter = 0;
}

static void Timer1Start(void)
{
	ET1 = 1;//����ʱ��1�ж�
	TR1 = 1;//������ʱ��1
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
	EA = 1; //�����ж�
	//hs_printf(" Main Open !\n\r");
	while(1);
}

//�ⲿ�ж�1
static void INIT1IRQHandler() interrupt 2
{
	//if(HightOut) //�����������ߵ�ƽ�����ж�
	//{
		//Timer0Stop();
		//Timer1Stop();
		//INIT1Start();
		//HightOut = 1;
	//}else
	{
		_Delay(100);
		INIT1Stop();
		Timer0Start();//����Time0��ʱ�����ƽ
		HightOut = 1; // ����ߵ�ƽ
	}
	
}

//��ʱ��T0 �ж���Ӧ
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

//������T1 �ж���Ӧ
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