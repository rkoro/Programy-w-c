#include <aduc831.h>

int i;
void tim(void) interrupt 3
{


P2^=(1<<4);

}

void main()
{
P2=0x00;
TMOD=0x20;
TR1=1;
TH1=0x6A;
TL1=0x00;
EA=1;//w³¹czenie przerwañ
ET1=1;//przerw od timera

while(1);

}