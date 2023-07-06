/*контроллер беговой дорожки

PA0,PA1,PA2 - выход управления скоростью (РА0 - младший, активный 1)
PA3 - выход управления реле (активный 1)
PA4, PD2 - вход датчика шкива
PA5,PA6 - резерв
PA7 - вход 12В

PC0 - кнопка режим + (активный 1)
PC1 - кнопка режим - (активный 1)
PC2 - кнопка скорость + (активный 1)
PC3 - кнопка скорость - (активный 1)
PC4, PD3 - вход измерения пульса
PC5 - выход управления пульсом
PC6 - кнопка сброс (активный 1)
PC7 - кнопка пуск/стоп (активный 1)

PB - выход на индикатор 
бит0 - верхний сегмент
бит1 - правый верхний сегмент
бит2 - правый нижний сегмент
бит3 - нижний сегмент
бит4 - левый нижний сегмент
бит5 - левый верхний сегмент
бит6 - средний сегмент
бит7 - точка

PD0 - RxD 232/485
PD1 - TxD 232/485
PD2 - Tx/Rx 485 (вход датчика шкива)
PD3 - зуммер (вход пульса)
PD4 - разряд индикатора 1
PD5 - разряд индикатора 2
PD6 - разряд индикатора 3
PD7 - разряд индикатора 4
*/

#include <iom32.h>
#include <intrinsics.h>
#define DELAY 10000

//functions declare;
void initialize(void);					//функция инициализации

//variables declare
unsigned char cg[21]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0x88,0x83,0xC6,0xA1,0x86,0x8E,0xFF,0xC1,0xC7,0x8C,0x87};
unsigned char buffer[4]={0,1,2,3},dot=5,zummer=0;
unsigned char point=0,displ=0,speed=1,start=0,key1=0,key2=0,rot=0,cursp=0,count=0,pulse=0,countp=0;
unsigned char minh=0,minl=0,sech=0,secl=0,lenh=0,lenm=0,lenl=0,sph=0,spm=0,spl=0;
unsigned int i;

//main function
void main(void)
{
 	initialize();
	
	while(1)
	{
		switch(displ)
		{
			case 0: //time
		  	{
				buffer[0]=minh;
				buffer[1]=minl;
				buffer[2]=sech;
				buffer[3]=secl;
				dot=1;
		  		break;
		  	}
			case 1: //speed
		  	{
				buffer[0]=17;
				buffer[1]=16;
				spl=0;
				i=speed;
				while(i>9)
				{
					i-=10;
					spl++;
				}
				
				buffer[2]=spl;
				buffer[3]=i;
				dot=4;
				break;
			}
			case 2: //length
			{
		  		buffer[0]=18;
				buffer[1]=lenh;
				buffer[2]=lenm;
				buffer[3]=lenl;
				dot=1;
		  		break;
		  	}
			case 3: //pulse
		  	{
				buffer[0]=19;
				sph=0;
				spm=0;
				spl=0;				
			  	i=pulse;
				//i<<=4;
			  	while(i>999)
				{
					i-=1000;
					sph++;
				}
			  	while(i>99)
				{
					i-=100;
					spm++;
				}
				while(i>9)
				{
					i-=10;
					spl++;
				}
				buffer[1]=sph;
				buffer[2]=spm;
				buffer[3]=spl;
				dot=4;
		  		break;
		  	}
		}
		
		if(speed>(cursp+1))
		{
			PORTA|=0x01;
			__delay_cycles(DELAY);
			PORTA|=0x04;
			__delay_cycles(DELAY);
			PORTA&=0xFB;
			__delay_cycles(DELAY);
			PORTA&=0xFE;
		}
		
		if(speed<(cursp-1))
		{
			PORTA|=0x02;
			__delay_cycles(DELAY);
			PORTA|=0x04;
			__delay_cycles(DELAY);
			PORTA&=0xFB;
			__delay_cycles(DELAY);
			PORTA&=0xFD;
		}
		
		__delay_cycles(DELAY);
	}
}

//initialize function
void initialize(void)
{
	//init ports
	DDRA=0x0F;
	PORTA=0x00;
	
	DDRB=0xFF;
	PORTB=0x00;
	
	DDRC=0x20;
	PORTC=0x20;
	
	DDRD=0xF0;
	PORTD=0xF0;
	  
	//timers
	TCCR0=0x04;
    TCCR1A=0x00;
    TCCR1B=0x0C;
    OCR1A=0xDA7A;
		
	//init interrupt
	TIMSK=0x11;
	MCUCR=0x0A;
	GICR=0xC0;
	SREG|=128;
	return;
}

#pragma vector=TIMER0_OVF_vect
__interrupt void timer0(void)
{
	PORTB=0xFF;
	point++;
	point&=0x03;
	PORTB=cg[buffer[point]];
    if(point==dot)
   		PORTB&=0x7F;
	PORTD|=0xF0;
	PORTD^=(0x01<<(point+4));
    	
	key1=PINC&0xCF;
	if(!key1)
	  key2=0;
	if(key1&&(key1!=key2))
	{
		key2=key1;
		if(key1&0x01)
		  displ++;
		if(key1&0x02)
		  displ--;
		if(key1&0x04)
		{
		  speed++;
		  if(speed>30)
			speed=30;
		}
		if(key1&0x08)
		{
		  speed--;
		  if(speed<1)
			speed=1;
		}
		if(key1&0x40)
		{
			minh=0;
			minl=0;
			sech=0;
			secl=0;
			lenh=0;
			lenm=0;
			lenl=0;
			start=0;
			PORTA&=0xF7;
		}
		if(key1&0x80)
		{
		  start=1;
		  PORTA|=0x08;
		}
		displ&=0x03;
	}
	return;
}

#pragma vector=TIMER1_COMPA_vect
__interrupt void timer1(void)
{
	if(start)
		secl++;
	if(secl>9)
	{
		secl=0;
		sech++;
	}
	if(sech>5)
	{
		sech=0;
		minl++;        
	}
	if(minl>9)
	{
		minl=0;
		minh++;
	}
	if(minh>9)
		minh=0;
	
	cursp=count;
	count=0;	
	pulse=countp;
	countp=0;
	return;
}

#pragma vector=INT0_vect
__interrupt void int0(void)
{
	GICR=0x00;
	GIFR=0x00;
  	if(start)
		rot++;
	rot&=0x3F;
	if(!rot)
		lenl++;
	if(lenl>9)
	{
		lenl=0;
		lenm++;
	}
	if(lenm>9)
	{
		lenm=0;
		lenh++;        
	}
	if(lenh>9)
	{
		lenl=0;
		lenm=0;
		lenh=0;
	}
	count++;
	GICR=0x40;
	return;
}

#pragma vector=INT1_vect
__interrupt void int1(void)
{
	countp++;
	return;
}
