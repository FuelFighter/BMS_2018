/*
 * Monitor_code.c
 *
 * Created: 3/10/2018 11:17:31 AM
 * Author : Vemund Rogne
 */ 

#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>
#include "pinDefinitions.h"
#include "LTC68042.h"
#include "Functions.c"
#include "LTC68042.c"

int main(void)
{
	init_LTC_SPI();
	set_adc(MD_NORMAL, DCP_DISABLED, CELL_CH_ALL, AUX_CH_GPIO1);
    
    while (1) 
    {
		LTC_wakeup_sleep();
		_delay_us(50);
		LTC6804_CS_P &= ~(1<<LTC6804_CS);
		_delay_us(10);
		LTC6804_CS_P |= (1<<LTC6804_CS);
		_delay_ms(5000);
    }
}

