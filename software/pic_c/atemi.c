/*
 * File:   atemi.c
 * Author: zakharov
 *
 * Created on 9 ????? 2017 ?., 12:14
 */

// PIC16F84A Configuration Bit Settings

// 'C' source line config statements

// CONFIG
#pragma config FOSC = HS    // Oscillator Selection bits
#pragma config WDTE = OFF   // Watchdog Timer
#pragma config PWRTE = OFF  // Power-up Timer Enable bit
#pragma config CP = OFF     // Code Protection bit

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>

unsigned char prot=255,cnt=0,speed=0,cmd1=0,cmd2=0,prt=0;

void main(void) {
    //initialize
    TRISA=0xFE;
    //TRISB=0xF5;
    TRISB=0xFD;
    PORTA=0x00;
    PORTB=0x00;
    
    while(1)
    {
        cnt++;
        prt=PORTA&0x0E;
        if(prt==2)
            cmd1|=1;
        if(prt==4)
            cmd2|=1;
        if((prt==10)&&(cmd1==1))
            cmd1|=2;
        if((prt==12)&&(cmd2==1))
            cmd2|=2;
        if((prt==2)&&(cmd1==3))
            cmd1|=4;
        if((prt==4)&&(cmd2==3))
            cmd2|=4;
        if((prt==0)&&(cmd1==7))
        {
            speed++;
            if(speed==0)
                speed=255;
            cmd1=0;
        }
        if((prt==0)&&(cmd2==7))
        {
            speed--;
            if(speed==255)
                speed=0;
            cmd2=0;
        }
        if(!RB0)
            speed=0;
                
        if((cnt==0)&&(RB0==1)&&(speed))
            RA0=1;
        if((cnt==speed)&&(cnt!=255))
            RA0=0;
        
        if(prot)
        {
            prot--;
            RB1=0;
        }
        else
            RB1=1;
        if(!RB2)
            prot=255;
    }
    return;
}
