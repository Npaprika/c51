//机器人智能循迹
//机器人从启动区出发
//沿黑色引导线A运行1圈
//数码管显示检测到的标志线1-8
#include <reg51.h>
//位定义
sbit P1_2=P1^2;
sbit P1_3=P1^3;
sbit P1_4=P1^4;
sbit P1_5=P1^5;
sbit P1_6=P1^6;
sbit P1_7=P1^7;
sbit P3_3=P3^3;
sbit P3_4=P3^4;
sbit P3_7=P3^7;
//宏定义
#define uchar unsigned char
#define uint  unsigned int
//左右红外传感器引脚
#define Left_IRSenor_Track   P3_4  
#define Right_IRSenor_Track  P3_3  
//速度值和左右电机PWM输入引脚  
#define SPEED               10	 
#define Left_Motor_PWM	   P1_4	 
#define Right_Motor_PWM	   P1_5	 
//左右车轮运动方向	
#define Left_Motor_Go      {P1_2=0,P1_3=1;} 
#define Left_Motor_Back    {P1_2=1,P1_3=0;} 
#define Left_Motor_Stop    {P1_2=0,P1_3=0;}                      
#define Right_Motor_Go     {P1_6=1,P1_7=0;}	
#define Right_Motor_Back   {P1_6=0,P1_7=1;}	
#define Right_Motor_Stop   {P1_6=0,P1_7=0;}	
//左右电机PWM值调节变量
unsigned char Left_PWM_Value=0;
unsigned char Left_Drive_Value=0;
unsigned char Right_PWM_Value =0;
unsigned char Right_Drive_Value=0;
bit Left_moto_stop =1;
bit Right_moto_stop=1;
//数码管锁存器选择  	
sbit POSSEL = P2^7;
sbit SEGSEL = P2^6;
//数码管位选码表、段码表
uchar POSCode[]={0xff,0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f};
uchar code SEGCode[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,
0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71,0x00};
//机器人智能循迹标志
unsigned char Line=0; 
unsigned char Flag;  
//毫秒延时
void delay_ms(unsigned int n)
{
    unsigned int i,j;
	   for(i=n; i>0; i--)
	      for(j=114; j>0; j--);
}
///数码管显示
void LEDTube_Show(unsigned char i,unsigned char j)
{
	P0=POSCode[i];   
	POSSEL=1;	  
	POSSEL=0;																
	P0=SEGCode[j];	 
	SEGSEL=1;	 
	SEGSEL=0;			
}
//机器人向前行驶
void GoForward(void)
{
    Left_Drive_Value=SPEED;	 
	Right_Drive_Value=SPEED; 
	Left_Motor_Go;  
	Right_Motor_Go; 
}
//机器人向左转
void GoLeft(void)
{			 
    Left_Drive_Value=SPEED;   
	Right_Drive_Value=SPEED; 
	Right_Motor_Go;  
    Left_Motor_Back; 
}
//机器人向右转
void GoRight(void)
{ 
    Left_Drive_Value=SPEED;   
	Right_Drive_Value=SPEED;  
    Left_Motor_Go;    
	Right_Motor_Back; 
}
//机器人停止运行
void Stop(void)	    
{	 
    Left_Motor_Stop;    
	Right_Motor_Stop;   
}
//机器人180度调头
void GoAround(void)
{
    Left_Drive_Value=11; 	
	Right_Drive_Value=11; 
	Left_Motor_Go;	      
	Right_Motor_Back;	   
	delay_ms(650);		   
	Stop();			  
}
//机器人后退
void GoBack(void)
{	 
    Left_Drive_Value=SPEED;   
	Right_Drive_Value=SPEED;  
	Left_Motor_Back;  
	Right_Motor_Back;   
}
//调节左电机转速函数
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
//调节右电机转速函数
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
//机器人采集红外循迹传感器数据控制电机智能循迹运行
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
//初始化定时器T0方式1
void TIMER0_Init(void)
{
	TMOD=0x01;       
	TH0=0xFC; 
    TL0=0x18;     	
    TR0=1;   
    ET0=1;	  
	EA=1;	   
}
//机器人沿引导环线智能循迹，数码管实时显示标志线根数
void main(void)
{
	unsigned char i=0; 
    P1=0xF0;   //关电机
	//机器人按键启动	        
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
//定时器0中断函数完成PWM对电机速度调节
void TIMER0_IRQHandler(void) interrupt 1 using 2
{
     TH0=0xFC;	  
	 TL0=0x18;
	 Left_PWM_Value++; 
	 Right_PWM_Value++;
	 Left_Motor_PWM_Adjust();
	 Right_Motor_PWM_Adjust();
}
