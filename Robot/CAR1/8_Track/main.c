//����������ѭ��
//�����˴�����������
//�غ�ɫ������A����1Ȧ
//�������ʾ��⵽�ı�־��1-8
#include <reg51.h>
//λ����
sbit P1_2=P1^2;
sbit P1_3=P1^3;
sbit P1_4=P1^4;
sbit P1_5=P1^5;
sbit P1_6=P1^6;
sbit P1_7=P1^7;
sbit P3_3=P3^3;
sbit P3_4=P3^4;
sbit P3_7=P3^7;
//�궨��
#define uchar unsigned char
#define uint  unsigned int
//���Һ��⴫��������
#define Left_IRSenor_Track   P3_4  
#define Right_IRSenor_Track  P3_3  
//�ٶ�ֵ�����ҵ��PWM��������  
#define SPEED               10	 
#define Left_Motor_PWM	   P1_4	 
#define Right_Motor_PWM	   P1_5	 
//���ҳ����˶�����	
#define Left_Motor_Go      {P1_2=0,P1_3=1;} 
#define Left_Motor_Back    {P1_2=1,P1_3=0;} 
#define Left_Motor_Stop    {P1_2=0,P1_3=0;}                      
#define Right_Motor_Go     {P1_6=1,P1_7=0;}	
#define Right_Motor_Back   {P1_6=0,P1_7=1;}	
#define Right_Motor_Stop   {P1_6=0,P1_7=0;}	
//���ҵ��PWMֵ���ڱ���
unsigned char Left_PWM_Value=0;
unsigned char Left_Drive_Value=0;
unsigned char Right_PWM_Value =0;
unsigned char Right_Drive_Value=0;
bit Left_moto_stop =1;
bit Right_moto_stop=1;
//�����������ѡ��  	
sbit POSSEL = P2^7;
sbit SEGSEL = P2^6;
//�����λѡ��������
uchar POSCode[]={0xff,0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f};
uchar code SEGCode[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,
0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71,0x00};
//����������ѭ����־
unsigned char Line=0; 
unsigned char Flag;  
//������ʱ
void delay_ms(unsigned int n)
{
    unsigned int i,j;
	   for(i=n; i>0; i--)
	      for(j=114; j>0; j--);
}
///�������ʾ
void LEDTube_Show(unsigned char i,unsigned char j)
{
	P0=POSCode[i];   
	POSSEL=1;	  
	POSSEL=0;																
	P0=SEGCode[j];	 
	SEGSEL=1;	 
	SEGSEL=0;			
}
//��������ǰ��ʻ
void GoForward(void)
{
    Left_Drive_Value=SPEED;	 
	Right_Drive_Value=SPEED; 
	Left_Motor_Go;  
	Right_Motor_Go; 
}
//����������ת
void GoLeft(void)
{			 
    Left_Drive_Value=SPEED;   
	Right_Drive_Value=SPEED; 
	Right_Motor_Go;  
    Left_Motor_Back; 
}
//����������ת
void GoRight(void)
{ 
    Left_Drive_Value=SPEED;   
	Right_Drive_Value=SPEED;  
    Left_Motor_Go;    
	Right_Motor_Back; 
}
//������ֹͣ����
void Stop(void)	    
{	 
    Left_Motor_Stop;    
	Right_Motor_Stop;   
}
//������180�ȵ�ͷ
void GoAround(void)
{
    Left_Drive_Value=11; 	
	Right_Drive_Value=11; 
	Left_Motor_Go;	      
	Right_Motor_Back;	   
	delay_ms(650);		   
	Stop();			  
}
//�����˺���
void GoBack(void)
{	 
    Left_Drive_Value=SPEED;   
	Right_Drive_Value=SPEED;  
	Left_Motor_Back;  
	Right_Motor_Back;   
}
//��������ת�ٺ���
void Left_Motor_PWM_Adjust(void)
{  
   if(Left_moto_stop)
	{
   		if(Left_PWM_Value<=Left_Drive_Value)
     		{
	    		 Left_Motor_PWM=1;
	   		}
		else 
    		{
        		 Left_Motor_PWM=0;
	   		}	
		if(Left_PWM_Value>=20)
       		Left_PWM_Value=0;
	}
   else    
    {
        Left_Motor_PWM=0;
	}
}
//�����ҵ��ת�ٺ���
void Right_Motor_PWM_Adjust(void)
{ 
  if(Right_moto_stop)
   { 
    	if(Right_PWM_Value<=Right_Drive_Value)
	    	{
	    	  	Right_Motor_PWM=1;
			}
		else 
	    	{
		   		Right_Motor_PWM=0;
		  	}
		if(Right_PWM_Value>=20)
			Right_PWM_Value=0;
   	}
   else    
    {
  		 Right_Motor_PWM=0;
    }	
}
//�����˲ɼ�����ѭ�����������ݿ��Ƶ������ѭ������
void Track(void)
{
    if(Left_IRSenor_Track==0&&Right_IRSenor_Track==0)
    {
        GoForward();
        Flag=1;		
    }	           
    if(Left_IRSenor_Track==0&&Right_IRSenor_Track==1)    
    {				 	 
        GoRight();
    }
    if(Left_IRSenor_Track==1&&Right_IRSenor_Track==0) 
    {	  				    
        GoLeft();
    }
    if(Left_IRSenor_Track==1&&Right_IRSenor_Track==1)
    {
        GoForward();		
        if(Flag!=0)
		{
			Line++;  
		}
		Flag=0;	  
    }
}
//��ʼ����ʱ��T0��ʽ1
void TIMER0_Init(void)
{
	TMOD=0x01;       
	TH0=0xFC; 
    TL0=0x18;     	
    TR0=1;   
    ET0=1;	  
	EA=1;	   
}
//��������������������ѭ���������ʵʱ��ʾ��־�߸���
void main(void)
{
	unsigned char i=0; 
    P1=0xF0;   //�ص��
	//�����˰�������	        
	while(1)   
	{
		if(P3_7!=1)
		{
			delay_ms(50);
			if(P3_7!=1) break;
		}
	}
	delay_ms(50);   
	TIMER0_Init();	 
	while(1)
	{ 
		if(Line<8)     
		{
			Track(); 	 
			if(Line)
				LEDTube_Show(6,Line);
		}
		else
		{
			Stop();	
		}
	}
}
//��ʱ��0�жϺ������PWM�Ե���ٶȵ���
void TIMER0_IRQHandler(void) interrupt 1 using 2
{
     TH0=0xFC;	  
	 TL0=0x18;
	 Left_PWM_Value++; 
	 Right_PWM_Value++;
	 Left_Motor_PWM_Adjust();
	 Right_Motor_PWM_Adjust();
}
