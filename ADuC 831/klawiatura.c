#include <aduc831.h>
#include <stdio.h>

#define LCD_DATA P0	
#define LCD_RS MCO
#define LCD_RW MDO

char a=0;
char tmp=0;
char tab[20];
char pom=0;
int liczba;



sbit LCDE = P2^0;

void delay(int x)
{
int a=0;
 for (a=0;a<x*12;a++);

}


void lcd_send(int p0,int mc0, int md0)
{
	  LCDE = 1;
	  LCD_RS = mc0;
	  LCD_RW = md0;

	  LCD_DATA = p0;

	  delay(100);

	  LCDE = 0;


}
char putchar(char a)
{
 	
	lcd_send(a,1,0);
	
	return a;
	
}


char klawiatura(void)
{
tmp=0;
while(tmp==0)
{
P3=0xDF;
if(~P1&(1<<4))
{
while(~P1&(1<<4))

tmp='1';
}
if(~P1&(1<<5))
{
while(~P1&(1<<5))

tmp='4';
}
if(~P1&(1<<6))
{
while(~P1&(1<<6))

tmp='7';
}
if(~P1&(1<<7))
{
while(~P1&(1<<7))

tmp='*';
}


P3=0xBF;
if(~P1&(1<<4))
{
while(~P1&(1<<4))

tmp='2';
}
if(~P1&(1<<5))
{
while(~P1&(1<<5))

tmp='5';
}
if(~P1&(1<<6))
{
while(~P1&(1<<6))

tmp='8';
}
if(~P1&(1<<7))
{
while(~P1&(1<<7))

tmp='0';
}


P3=0x7F;
if(~P1&(1<<4))
{
while(~P1&(1<<4))

tmp='3';
}
if(~P1&(1<<5))
{
while(~P1&(1<<5))

tmp='6';
}
if(~P1&(1<<6))
{
while(~P1&(1<<6))

tmp='9';
}
if(~P1&(1<<7))
{
while(~P1&(1<<7))

tmp='\n';
}


}
return(tmp);
}


char _getkey()
{
return klawiatura();
}
void main(void)
{

MDE = 1;
I2CM = 1;

delay(50);

lcd_send(1,0,0);

lcd_send(2,0,0);

lcd_send(15,0,0);
lcd_send(50,0,0);
	



P1=0x00;
P3=0xFF;

while(1)
{


scanf("%d",&liczba);
printf("x2= %d", liczba*2);
}

}
