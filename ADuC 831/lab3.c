#include<aduc832.h>
#include<stdio.h>

  #define LCD_DATA P0
  sbit LCD_E=P2^0;
  #define LCD_RS MCO
  #define LCD_RW MDO

		int i;
	void delay(void)
	{
		int b;
		for (b=0; b<10000; b++);
	}
/*
	void timer(int tf1, int tr1, int tf0, int tr0)
	{
		TF1=tf1;
		TR1=tr1;
		TF0=tf0;
		TR0=tr0;
	}
	*/
  void lcd_wyslij(int rs, int rw, char dane)
  {
		LCD_E=1 ;		//rozpoczecie transmisji
		delay();	 /* jakieœ opóznienie */
		LCD_RS=rs;
		LCD_RW=rw;
		LCD_DATA=dane;
		delay();
		LCD_E=0; /* zakoñæzenie transmisji */

  }
void tajmery(void) interrupt 3 
	{
		
	//i++;
	//	if(i>10)
	//		P3=0x3F;
		TH1=128;
	if(i== 0){
		
		P2= 1<<4;
	}
	else{
		P2= 0;
	}
	i= ~i;
}
	
	
  char putchar(char c)
	{
	 lcd_wyslij(1, 0, c);
	 return c;
	}
  void main(void)
  {

  MDE=1;		// uzyskanie dostepu do pino RS oraz RW, czyli transmisja z wyswietlaczem
  I2CM=1;		// jedna uruchamia magistrale, a druga... cos tam

		 lcd_wyslij(0, 0, 0x01);
		 delay()  ;

		 lcd_wyslij(0, 0, 0x02);
		 delay();

		 lcd_wyslij(0, 0, 0x0F);
		 delay();

		 lcd_wyslij(0, 0, 0x3F)	;
		 delay();
			 // rejestry 4xTMOD starsze, 4xTCON mlodsze
	//	printf("tekst1") ;
		
		//timer(0,1,0,0);
		
		
		//porty odpowiedzialne za prace timera, rejsetry wykorzystane sa wypisane na stronach 50,51 
			P3= 0xFF;
			P2= 0xFF;
		  TMOD= 0x10; 
			TR1= 1;
	/*
			for(i=0 ; i<50; i++)
			{	
				TF1= 0;
				TL1= 0;
				TH1= 0; //TH1 sa to 2 bajty do przechowywania stanu licznika

				if(TF1)
				{
					P2= ~(P2<<4);
					TF1=0;
					
				}
				//while(!TF1)	;
			}
		*/
			P3=0x00;
		
		
		TL1= 0;
		TH1= 0;
		
		EA=1; //wlaczenie wszystkichj przerwan
		ET1=1; //wlaczenie konkretnego przerwania
		
		
		
		while(1)
		{
			
		}
		 }

