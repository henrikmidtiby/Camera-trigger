/****************************************************************************
# RoboCard LED Test
# Copyright (c) 2010 Kjeld Jensen <kjeld@robocard.dk>
# 
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved. This file is offered as-is,
# without any warranty.
*****************************************************************************
# File: main.c                                              
# Project: RoboCard LED Test                                        
# Platform: RoboCard v1.0 http://www.robocard.dk                                                
# Microcontroller: ATmega88PA                                                      
# Author: Kjeld Jensen <kjen@robocard.dk>
# Created: 2010-08-10                                             
# Last modified: 2010-08-10                                          
****************************************************************************/
/* includes */
#include <avr/io.h>
#include <avr/interrupt.h>

/***************************************************************************/
/* defines */
#define false				0
#define true				1



/* adc defines */
#define ADC_NUM		1

/***************************************************************************/
/* variables */

volatile unsigned char adc_data[ADC_NUM]; /* adc data variables */
volatile unsigned char adc_ch; /* current adc channel */


/***************************************************************************/
void led(char on)
{
	if (on)
		PORTB &= ~(1<<PB0); /* set PB0 output to GND turning led on */
	else
		PORTB |= (1<<PB0); /* set PB0 output to VCC turning led off */
}
/***************************************************************************/
void led_init(void)
{
	led (0); /* make sure the led is off */
	DDRB |= (1<<DDB0); /* enable PB0 (led pin) as output */
}
/***************************************************************************/
/* ADC interrupt handler */
ISR (ADC_vect)
{
	adc_data[adc_ch] = ADCH; /* only read 8 bit (ADLAR is set in ADMUX) */

	if (++adc_ch >= ADC_NUM) /* go to next adc channel */
		adc_ch = 0;
	ADMUX = (1<<ADLAR) + adc_ch;

	ADCSRA |= (1<<ADSC); /* request adc conversion */
}
/***************************************************************************/
void adc_init (void)
{
	adc_ch = 0;

	ADCSRA = (1<<ADEN); /* enable ADC conversion */
	ADCSRA |= ((1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0)); /* div by 128 presc. */
	ADCSRA |= (1<<ADIE); /* interrupt enable */

	ADMUX = (1<<ADLAR); /* channel 0, left adjusted (voltage ref is AREF) */

	ADCSRA |= (1<<ADSC); /* request adc conversion */
}
/***************************************************************************/
int main(void)
{
	char led_stat = 0;
	long count = 0;

	led_init(); /* initialize led */
	adc_init(); /* Initialize adc */
	sei(); // %cli ?



	for (;;) /* go into an endless loop */
	{
		count++; /* increace count by one */
		 
		if(count / (10 + adc_data[0]) > 100) /* if time to switch led state */
		{
			count=0; /* reset count */
			if (led_stat == 0) /* flip led state */
				led_stat = 1;
			else
				led_stat = 0;
			led(led_stat); /* update led state */
		}
	}
	return 0; /* just for the principle as we never get here */
}
/***************************************************************************/
