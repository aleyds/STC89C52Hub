#include "stdio.h"
#include <reg52.h>
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

sbit HightOut = P2^0;
sbit TriggerIn = P2^1;

static unsigned int g_Timer0Counter = 0;
static unsigned int g_Timer1Counter = 0;
#define TIMER0_TIMEOUT		(3000) //300mS  ��ʱ300ms  ���3S�ߵ�ƽ
#define TIMER1_TIMEOUT		(30000) //3S

static void INIT0Start(void)
{
	EX0 = 1;//���ⲿ�ж�0
    IT0 = 0;//�жϴ�����ʽ
}

static void INIT0Stop(void)
{
	 EX0 = 0;//�����ж�0
}

static void TimerInit(void)
{
	TMOD = 0X22;//���ö�ʱ��������ʽT0,T1��Ϊ������ʽ2��8λ�Զ���װ��  
    TH0 = 0x7B;//16MHz ����  ��ʱ 100us
    TL0 = 0x7B;
    TH1 = 0x7B;
    TL1 = 0x7B;
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
	INIT0Start();
	TimerInit();
	EA = 1; //�����ж�
	while(1);
}

//�ⲿ�ж�0
static void INIT0IRQHandler() interrupt 0
{
	if(HightOut) //�����������ߵ�ƽ�����ж�
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

//��ʱ��T0 �ж���Ӧ
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

//������T1 �ж���Ӧ
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