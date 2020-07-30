#include <aduc831.h>
#include <stdio.h>

#define LCD_DATA P0
sbit LCD_E = P2^0;
#define LCD_RS MCO
#define LCD_RW MDO




void delay(int t)
{
   long int a;
   for(a=0; a<1000*t;a++);
}

void lcd_send(int dane, int rs, int rw)
{
	LCD_E=1;//enable communication
	LCD_RS=rs;
	LCD_RW=rw;
	LCD_DATA=dane;
	delay(100);// jakies opóznienie
	LCD_E=0;//koniec transmisji
}

void lcd_clear(void)
{
lcd_send(0x01,0,0);
}

void lcd_return(void)
{
lcd_send(0x02,0,0);
}

void lcd_on(void)
{
lcd_send(0x0F,0,0);
}

void lcd_mode(void)
{
 lcd_send(0x30,0,0);
}

char putchar(int b)
{
lcd_send(b,1,0);
return b;
}

void main(void)
{
int d;
MDE=1;
I2CM=1;//Przejêcie kontroli nad pinami od I2C
delay(500);
lcd_clear();
delay(500);
lcd_return();
delay(500);
lcd_on();
lcd_mode();

for(d=0;d<5;d++)
{
printf("abc");
}
while(1)
{
}
}

