#include <avr/io.h>
#include <util/delay.h>
#include "hd44780.h"
#include "stdio.h"


int main(void)
{
    lcd_init();
    LCD_DISPLAY(LCDDISPLAY);
	unsigned int i=0;
    while(1)
    {
		i++;
		LCD_CLEAR;
        lcd_puts_delay("Kocham cie", 200);
		LCD_LOCATE(4,1);
        lcd_puts_delay("puszyco :*", 200);
		
		unsigned char i=0;
		for(i=0; i<4; i++)
		{
			_delay_ms(500);
			LCD_CLEAR;
			
			_delay_ms(300);
			lcd_puts("Kocham cie");
			LCD_LOCATE(4,1);
			lcd_puts("puszyco :*");
			
		}
        _delay_ms(2800);  
        
    }
    return 0;
}