#include <aduc831.h>

void delay (int t)
{
long int a;
  for(a=0; a<t*1200;a++);
}

void main(void)
{

P3=0x00;
while(1)
{
delay(1);
P3|=(1<<5);
delay(1);
P3|=(1<<6);
delay(1);
P3|=(1<<7);
delay(1);
P3&=~(1<<7);
delay(1);
P3&=~(1<<6);
delay(1);
P3&=~(1<<5);
delay(1);



}
}