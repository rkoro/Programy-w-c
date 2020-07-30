#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "hd44780.h"
#include "stdio.h"
#include <stdlib.h>

#define SETBIT(ADDRESS,BIT) (ADDRESS |= (1<<BIT))
#define CLEARBIT(ADDRESS,BIT) (ADDRESS &= ~(1<<BIT))
#define FLIPBIT(ADDRESS,BIT) (ADDRESS ^= (1<<BIT))
#define GETBIT(ADRESS,BIT) (ADRESS&(1<<BIT))

#define ROWS 8
#define COLS 5
#define PLUSPORT PORTA
#define MINUSPORT PORTD

#define GORA 0
#define PRAWO 1
#define DOL 2
#define LEWO 3
#define BRAKKIERUNKU 4

#define GAMEOVER 0
#define OK 1

uint8_t rowAdresses[ROWS] = {0, 1, 2, 3, 4, 5, 6, 7};
uint8_t colAdresses[COLS] = {0, 1, 4, 5, 6};
uint8_t row, col;
volatile uint8_t timer=0;
volatile uint8_t kierunek = GORA;
volatile uint8_t gameover = 0;

#define KEY1 PINB, 0, &key_lock
uint8_t key_lock=0;
uint8_t przerwania=1;

#define MIN 1110
#define MAX 1930
#define SREDNIA 1520

volatile uint16_t rstop=0;
volatile uint16_t signaltime=0;
volatile uint16_t uptime=0;
uint16_t signaltimebuff=0;
uint16_t uptimebuff=0;

volatile uint16_t rstop2=0;
volatile uint16_t signaltime2=0;
volatile uint16_t uptime2=0;
uint16_t signaltimebuff2=0;
uint16_t uptimebuff2=0;

volatile uint8_t t0highbyte=0;
volatile uint8_t t2highbyte=0;

volatile uint16_t speed=3900;

uint8_t wynik, poprzednikierunek=GORA, nowywynik=10, wynikcounter=0;

void diode_off(uint8_t row, uint8_t col)
{
	CLEARBIT(PLUSPORT, rowAdresses[row]);
	SETBIT(MINUSPORT, colAdresses[col]);
}

void diode_on(uint8_t row, uint8_t col)
{
	SETBIT(PLUSPORT, rowAdresses[row]);
	CLEARBIT(MINUSPORT, colAdresses[col]);
}

struct lista
{
	uint8_t x;
	uint8_t y;
	struct lista *volatile next;
};

struct lista *volatile glowa=NULL;
struct lista *volatile last=NULL;
struct lista *volatile jablko=NULL;

uint8_t jablkoRandOk(uint8_t x, uint8_t y)
{
	struct lista *current;
	current = (struct lista*)glowa;
	do
	{
		if(current->x == x && current->y == y)
			return 0;

		current=current->next;
	}while(current!=NULL);

	return 1;
}

void losujJablko()
{
	uint8_t x,y;
	do
	{
		x=rand()%COLS;
		y=rand()%ROWS;
	}while(!jablkoRandOk(x, y));

	jablko->x = x;
	jablko->y = y;
}

uint8_t dead()
{
	struct lista *current = glowa->next;
	do
	{
		if(current->x == glowa->x && current->y == glowa->y)
			return 1;

		current=current->next;
	}while(current!=NULL);

	return 0;
}

char text[17];

uint8_t move()
{
	volatile uint8_t newx, newy;
	struct lista *almostLast;
	switch(kierunek)
	{
		case GORA:
		{
			newx=glowa->x;
			newy=glowa->y - 1;
		};break;

		case DOL:
		{
			newx=glowa->x;
			newy=glowa->y + 1;
		};break;

		case LEWO:
		{
			newx=glowa->x-1;
			newy=glowa->y;
		};break;

		case PRAWO:
		{
			newx=glowa->x+1;
			newy=glowa->y;
		}; break;

	}
	if(newx == COLS)
		newx=0;
	if(newx == 255)
		newx=COLS-1;
	if(newy == ROWS)
		newy=0;
	if(newy == 255)
		newy=ROWS-1;

	if(newx == jablko->x && newy==jablko->y)
	{
		jablko->next = (struct lista*)glowa;
		glowa = jablko;
		jablko = malloc(sizeof(struct lista));
		losujJablko(jablko);
		//lcd_puts("jablko");

		if(speed > 1000)
			speed -= 100;
	}
	else
	{
		last->x=newx;
		last->y=newy;
		last->next = (struct lista*)glowa;
		almostLast = (struct lista*)glowa;
		glowa = last;

		while(almostLast->next != last)
			almostLast = almostLast->next;

		almostLast->next = NULL;
		last = almostLast;
		//lcd_puts("gogo");
	}

	poprzednikierunek = kierunek;

	/*LCD_CLEAR;

	LCD_LOCATE(0,0);
	sprintf(text, "%d,%d %d,%d %d,%d", glowa->x, glowa->y, glowa->next->x, glowa->next->y,  glowa->next->next->x, glowa->next->next->y);
	lcd_puts(text);

	LCD_LOCATE(0,1);
	sprintf(text, "%d,%d new:%d,%d ", jablko->x,jablko->y,newx,newy);
	lcd_puts(text);*/

	if(dead())
		return GAMEOVER;

	return OK;
}

uint8_t isPressed(int8_t addr, uint8_t bit, uint8_t *lock)
{
	if( !*lock && !(addr & (1<<bit) ) )
	{
		*lock=0;
		return 1;
	} else if( *lock && (addr & (1<<bit) ) ) ++*lock;

	return 0;
}



uint8_t pobierzkierunek(uint16_t lp, uint16_t gd)
{
	//1315
	//1725

	if(abs(SREDNIA-lp)>abs(SREDNIA-gd))
	{
		if(lp<1315)
			return PRAWO;
		else if(lp>1725)
			return LEWO;
	}
	else
	{
		if(gd<1315)
			return DOL;
		else if(gd>1725)
			return GORA;
	}

	return BRAKKIERUNKU;
}




void steruj()
{
	if(gameover)
		return;

	cli();
	/*cli();
	uptimebuff = uptime;
	signaltimebuff = signaltime;
	uptimebuff2 = uptime2;
	signaltimebuff2 = signaltime2;
	sei();


	wynik=pobierzkierunek(uptimebuff2, uptimebuff);*/
	wynik=pobierzkierunek(uptime2, uptime);
	if(wynik != poprzednikierunek && wynik!=BRAKKIERUNKU)
	{
		if(wynikcounter>100)
		{
			if((wynik+2)%4 != poprzednikierunek)
			{
				kierunek = wynik;
				wynikcounter=0;
			}


			/*switch(wynik)
			{
				case GORA:
					if(kierunek != DOL)
						kierunek = GORA;

				break;

				case PRAWO:
					if(kierunek != LEWO)
						kierunek = PRAWO;
				break;

				case DOL:
					if(kierunek != GORA)
						kierunek = DOL;
				break;

				case LEWO:
					if(kierunek != PRAWO)
						kierunek = LEWO;
				break;
			}*/
		}
		else if(wynikcounter==0)
		{
			nowywynik = wynik;
			wynikcounter++;
		}
		else if(nowywynik == wynik)
		{
			wynikcounter++;
		}
		else
			wynikcounter=0;
	}
	else
		wynikcounter=0;

	sei();

}



int main(void)
{
	/*DDRB=0x00; //wejscie
	PORTB=0xFF; //pullup resistor
	_delay_ms(10);*/

	last = malloc(sizeof(struct lista));
	last->next=NULL;
	last->y=7;
	last->x=1;

	struct lista *el;
	el = malloc(sizeof(struct lista));
	el->next=(struct lista*)last;
	el->y=6;
	el->x=1;

	glowa = malloc(sizeof(struct lista));
	glowa->next=el;
	glowa->y=5;
	glowa->x=1;

	jablko = malloc(sizeof(struct lista));
	jablko->next = NULL;
	losujJablko(jablko);

	DDRA=0xFF; //wyjscie
	DDRD=0b01110011; //D0,D1,D4,D5,D6 wyjscia
	SETBIT(PORTD,2);	// Enable pull-up resistor
	SETBIT(PORTD,3);	// Enable pull-up resistor

	_delay_ms(20);

	TIMSK |= (1<<TOIE0) | (1<<TOIE2);// Timer/Counter0 and 2 Overflow Interrupt Enable

	TCCR0 |= (1<<CS01); //8 prescaler
	TCNT0 = 0x00;

	TCCR2 |= (1<<CS21); //8 prescaler
	TCNT2 = 0x00;

	GICR = 1<<INT0 | 1<<INT1;
	MCUCR |= 1<<ISC01 | 1<<ISC00;	// Trigger INT0 on rising edge
	MCUCR |= 1<<ISC11 | 1<<ISC10;	// Trigger INT1 on rising edge

	//3900
	TIMSK |= (1<<TOIE1);// Timer/Counter1 Overflow Interrupt Enable
	TCCR1B |= (1<<CS10)|(1<<CS12); //1024 prescaler 1 tykniecie 0,000128s.
	TCNT1 = 0xFFFF - speed; //65535 - 3900 czyli okolo co 0,4992 sec

	/*TIMSK |= (1<<TOIE0);// Timer/Counter0 Overflow Interrupt Enable
	TCCR0 |= (1<<CS00)|(1<<CS02); //1024 prescaler 1 tykniecie 0,000128s. caly przebieg 0,032768s
	TCNT0 = 0x00;*/

	for(row=0; row<ROWS; row++)
	{
		for(col=0; col<COLS; col++)
		{
			diode_off(row, col);
		}
	}

	_delay_ms(1);

    lcd_init();
    LCD_DISPLAY(LCDDISPLAY);
	sei();
	/*
	uint8_t wynik, poprzedniwynik=10;
	char text[17];
    while(1)
    {
		cli();
		uptimebuff = uptime;
		signaltimebuff = signaltime;
		uptimebuff2 = uptime2;
		signaltimebuff2 = signaltime2;
		sei();



		wynik=steruj(uptimebuff2, uptimebuff);
		LCD_LOCATE(0,0);
		sprintf(text, "%d/%d", uptimebuff2,uptimebuff);
		lcd_puts(text);
		if(wynik != poprzedniwynik)
		{
			LCD_CLEAR;
			LCD_LOCATE(0,1);
			switch(wynik)
			{
				case GORA: lcd_puts("GORA"); break;
				case PRAWO: lcd_puts("PRAWO"); break;
				case DOL: lcd_puts("DOL"); break;
				case LEWO: lcd_puts("LEWO"); break;
				case BRAKKIERUNKU: lcd_puts("BRAK KIERUNKU"); break;
			}
		}
		poprzedniwynik = wynik;
    }*/

	while(1)
	{
		/*if(isPressed(KEY1))
		{
			if(kierunek==GORA)
			{
				kierunek=PRAWO;
				lcd_puts("PRAWO ");
			}
			else if(kierunek==PRAWO)
			{
				kierunek=DOL;
				lcd_puts("DOL ");
			}
			else if(kierunek==DOL)
			{
				kierunek=LEWO;
				lcd_puts("LEWO ");
			}
			else
			{
				kierunek=GORA;
				lcd_puts("GORA ");
			}
			if(przerwania)
			{
				cli();
				przerwania=0;
			}
			else
			{
				sei();
				przerwania=1;
			}
		}*/

		steruj();

		el = (struct lista*)glowa;

		diode_on(el->y, el->x);
		_delay_us(200);
		diode_off(el->y, el->x);
		el=el->next;

		do
		{
			diode_on(el->y, el->x);
			_delay_us(10);
			diode_off(el->y, el->x);

			el=el->next;

		}while(el!=NULL);

		diode_on(jablko->y, jablko->x);
		_delay_us(1);
		diode_off(jablko->y, jablko->x);

		/*for(row=0; row<ROWS; row++)
		{
			for(col=0; col<COLS; col++)
			{
				if(tablica[row][col])
				{
					diode_on(row, col);
					_delay_ms(1);
					diode_off(row, col);
				}
			}
		}*/
	}
    return 0;
}

/*ISR(TIMER0_OVF_vect)
{
	if(timer >= 8) //0,0262144s
	{
		if(move()==GAMEOVER)
		{
			cli();
			lcd_puts("DEAD");
		}
		timer = 0;
	}
	else
		timer ++;
}*/

ISR(TIMER1_OVF_vect)
{
	if(!gameover && move()==GAMEOVER)
	{
		gameover = 1;
		lcd_puts("DEAD");
	}

	TCNT1 = 0xFFFF - speed;
}

ISR(INT0_vect)
{
	if(GETBIT(PIND,PD2))
	{
		CLEARBIT(MCUCR,ISC01); //next time falling edge triggering
		uptime = rstop;
		signaltime = ((t0highbyte << 8 ) | TCNT0);
		//TCNT1 = 0;
		TCNT0 = 0;
		t0highbyte = 0;

	}
	else
	{
		SETBIT(MCUCR,ISC01); //next time rising edge triggering
		rstop = ((t0highbyte << 8 ) | TCNT0);
	}
}

ISR(INT1_vect)
{
	if(GETBIT(PIND,PD3))
	{
		CLEARBIT(MCUCR,ISC11); //next time falling edge triggering
		uptime2 = rstop2;
		signaltime2 = ((t2highbyte << 8 ) | TCNT2);
		TCNT2 = 0;
		t2highbyte = 0;
	}
	else
	{
		SETBIT(MCUCR,ISC11); //next time rising edge triggering
		rstop2 = ((t2highbyte << 8 ) | TCNT2);
	}
}

ISR(TIMER0_OVF_vect) {
	t0highbyte++;
}

ISR(TIMER2_OVF_vect) {
	t2highbyte++;
}
