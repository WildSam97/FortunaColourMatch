/* For La Fortuna board
* Author: Sam Wild
* Makes use of Steve Gunn's led, lcd and rotary libraries, as well as a modified version of switches.c from Phill Raynsford's FortunaTetris
*/

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "lcd.h"
#include "led.h"
#include "rotary.h"
#include "switches.h"
#include <util/delay.h>


const rectangle startShip = {(LCDWIDTH-5)/2, (LCDWIDTH+5)/2, LCDHEIGHT-5-1, LCDHEIGHT-1};
const rectangle startGoal = {(LCDWIDTH-20)/2, (LCDWIDTH+20)/2, LCDHEIGHT-20-50, LCDHEIGHT-50};

volatile rectangle ship, lastShip, goal, lastGoal;
volatile uint8_t colour;	//colour used for square(max 6)
volatile uint16_t currentColour, goalColour;
volatile uint8_t timeLeft;
volatile uint8_t goalNeeded;
volatile uint8_t score;
volatile uint8_t moving;
volatile uint8_t mode;
volatile uint8_t menu;

uint16_t random_seed;

/* functions rand_init and rand are taken from Giacomo Meanti's
   Space Invaders game from 2014-2015
*/

uint16_t rand_init(void) {
    //ADC conversion from unused pins should give random results.
    //an amplification factor of 200x is used.
    ADMUX |= _BV(REFS0) | _BV(MUX3) | _BV(MUX1) | _BV(MUX0);
    //Prescaler
    ADCSRA |= _BV(ADPS2) | _BV(ADPS1);
    //Enable and start
    ADCSRA |= _BV(ADEN) | _BV(ADSC);
    //Wait until complete
    while(! (ADCSRA & _BV(ADIF))) {
        _delay_ms(2);
    }
    //Read result
    uint16_t res = ADCL;
    res |= ADCH << 8;
    //Disable
    ADCSRA &= ~_BV(ADEN);
    
    //The XOR should increase the randomness?
    //since the converted number is only 10 bits
    return res ^ 0xACE1u;
}

//http://en.wikipedia.org/wiki/Linear_feedback_shift_register
uint16_t rand(void) {
    unsigned lsb = random_seed & 1;  /* Get lsb (i.e., the output bit). */
    random_seed >>= 1;               /* Shift register */
    if (lsb == 1)             /* Only apply toggle mask if output bit is 1. */
        random_seed ^= 0xB400u;        /* Apply toggle mask, value has 1 at bits corresponding
                             * to taps, 0 elsewhere. */
    return random_seed;
}

ISR(INT6_vect)	
{
	fill_rectangle(lastGoal, display.background);
	fill_rectangle(goal, goalColour);
	fill_rectangle(lastShip, display.background);
	fill_rectangle(ship, currentColour);
	lastShip=ship;
	lastGoal=goal;
}

ISR(TIMER1_COMPA_vect)
{
	static int8_t xinc = 1, yinc = 1;
	if (rotary<0){
		colour--;
		if(colour<1){
			colour=14;
		}
	}
	if (rotary>0){
		colour++;
		if(colour>14){
			colour=0;
		}
	}
	rotary=0;	
	switch(colour){
		case 2:
			currentColour=WHITE;
		break;
		case 4:
			currentColour=BLUE;
		break;
		case 6:
			currentColour=GREEN;
		break;
		case 8:
			currentColour=CYAN;
		break;
		case 10:
			currentColour=RED;
		break;
		case 12:
			currentColour=MAGENTA;
		break;
		case 14:
			currentColour=YELLOW;
		break;		
	}
	
	if (up_held() && ship.top >= 1+20){
		ship.top -= 1;
		ship.bottom -= 1;
	}
	
	if (down_held() && ship.bottom < display.height-1-1){
			ship.top += 1;
			ship.bottom += 1;
	}
	
	if (left_held() && ship.left >=1){
			ship.left -= 1;
			ship.right -= 1;
	}
	if (right_held() && ship.right < display.width-1){
			ship.left += 1;
			ship.right += 1;
	}
	
	if(goalNeeded){
		uint8_t random;
		uint8_t goalx;
		uint8_t goaly;
		random = (rand()%6)+1;
		goalx=(rand()%(LCDWIDTH-20));
		goaly=20+(rand()%(LCDHEIGHT-40));
		while(goalx<0){
			goalx=(rand()%(LCDWIDTH-20));
		}
		while(goaly<20){
			goaly=20+(rand()%(LCDHEIGHT-40));
		}
		
		switch(random){
			case 1:
				goalColour=WHITE;
			break;
			case 2:
				goalColour=BLUE;
			break;
			case 3:
				goalColour=GREEN;
			break;
			case 4:
				goalColour=CYAN;
			break;
			case 5:
				goalColour=RED;
			break;
			case 6:
				goalColour=MAGENTA;
			break;
			case 7:
				goalColour=YELLOW;
			break;		
		}
		goal.top=goaly;
		goal.left=goalx;
		goal.right=goalx+20;
		goal.bottom=goaly+20;
		goalNeeded=0;
		if(mode){
			moving=1+(rand()%2);
		}
		
		
	}
	if(moving==2){
		goal.left   += xinc;
		goal.right  += xinc;
		goal.top    += yinc;
		goal.bottom += yinc;
		if (goal.right >= display.width-1 || goal.left <= 0){
			xinc = -xinc;
		}
		if (goal.top <= 20 || goal.bottom >= display.height-1){
			yinc = -yinc;
		}
	}
	if(ship.top>=goal.top && ship.bottom<=goal.bottom && ship.left>=goal.left && ship.right <=goal.right){
		if(currentColour == goalColour){
			timeLeft+=5;
			goalNeeded=1;
			score++;
			xinc=1;
			yinc=1;
		}
	}
	
	
}
ISR(TIMER3_COMPA_vect)
{
	char buffer[4];
	sprintf(buffer, "%03d", timeLeft);
	display_string_xy("time left:", 1, 1);
	display_string_xy(buffer, 70, 1);
	timeLeft--;
	sprintf(buffer, "%03d", score);
	display_string_xy("score:", 100, 1);
	display_string_xy(buffer, 150, 1);
}

int main(){
	CLKPR = (1 << CLKPCE);
	CLKPR = 0;
	init_rotary();
	init_lcd();
	set_frame_rate_hz(61);
	/* Enable tearing interrupt to get flicker free display */
	EIMSK |= _BV(INT6);
	/* Enable rotary interrupt to respond to input */
	EIMSK |= _BV(INT4) | _BV(INT5);
	/* Enable game timer interrupt (Timer 1 CTC Mode 4) */
	TCCR1A = 0;
	TCCR1B = _BV(WGM12);
	TCCR1B |= _BV(CS10);
	TIMSK1 |= _BV(OCIE1A);
	/* Enable game countdown timer (Timer 3 CTC Mode 4) */
	TCCR3A = 0;
	TCCR3B = _BV(WGM32);
	TCCR3B |= _BV(CS32);
	TIMSK3 |= _BV(OCIE3A);
	OCR3A = 11250;
	
	random_seed=rand_init();
	do {
		menu=1;
		lastShip = ship = startShip;
		lastGoal = goal = startGoal;
		timeLeft = 60;
		colour=2;
		goalNeeded=1;
		score=0;
		moving=0;
		OCR1A = 65535;
		led_on();
				display_string_xy("Press Left for Easy Mode", 20, 20);
				display_string_xy("Press Right for Hard Mode", 20, 40);
		while(menu){
			if(left_pressed()){
				mode=0;
				menu=0;
			}
			if(right_pressed()){
				mode=1;
				menu=0;
			}
		}
		clear_screen();
		sei();
		while(timeLeft);
		cli();
		led_off();
		display_string_xy("Game Over", 90, 150);
		display_string_xy("Press Centre Button To Return To Menu", 10, 170);
		PORTB |= _BV(PB6);
		while(PINE & _BV(SWC))
		{
			if (PINB & _BV(PB6))
				led_on();
			else
				led_off();
		}
		clear_screen();
	} while(1);
}

