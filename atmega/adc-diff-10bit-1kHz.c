#include <avr/io.h>
#include <avr/interrupt.h>

#define UART_BAUD	250000
#define UART_CONST	(F_CPU/16/UART_BAUD - 1)

void uart_init(void)
{
	/* Set baudrate */
	UBRRH = (uint8_t)(UART_CONST >> 8);
	UBRRL = (uint8_t)(UART_CONST);
	
	/* Enable transmitting */
	UCSRB = _BV(TXEN);
	
	/* Set data size and type for 8N1 */
	UCSRC = _BV(URSEL) | _BV(UCSZ0) | _BV(UCSZ1);
}

void uart_transmit(uint8_t data)
{
	/* wait for empty buffer */
	while(!(UCSRA & _BV(UDRE)));
	
	/* put data to send into uart buffer */
	UDR = data;
}

void adc_init(void)
{
	/* reference voltage as avcc */
	ADMUX |= _BV(REFS0);
	ADMUX |= _BV(MUX4);

	/* enable adc */
	ADCSRA |= _BV(ADEN);
	
	/* adc clock prescaler 128: 16MHz/128 = 125kHz */
	ADCSRA |= _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);	
}

void adc_get_sample(uint16_t* out)
{
	/* begin conversion */
	ADCSRA |= _BV(ADSC);
	
	/* wait for result and return it */
	while(!(ADCSRA & ADIF));
	*out = ADC;
}

// void adc_get_sample(uint8_t* out)
// {
	// /* begin conversion */
	// ADCSRA |= _BV(ADSC);
	
	// /* wait for result and return it */
	// while(!(ADCSRA & ADIF));
	// *out = ADCH;
// }

void timer_init(void)
{
	/* enable timer1 interrupt */
	TIMSK = _BV(TOIE1);
	/* initialise it with proper value (250 cycles till overflow) */
	TCNT1 = 0xFF06;
	/* set timer1 mode to normal mode */
	TCCR1A = 0x00;
	/* set timer1 prescaler to 64: 16MHz/64 = 250kHz */
	TCCR1B = _BV(CS11) | _BV(CS10);	
}


int main(void)
{
	MCUCR |= _BV(ISC01);
	GICR |= _BV(INT0);

	uart_init();
	adc_init();
	timer_init();
	sei();
	
	// ADMUX |= _BV(ADLAR);

	while(1);
}

uint8_t mode = 0;

ISR(INT0_vect)
{
	
	// wyzeruj stan
	ADMUX &= ~(_BV(MUX0) | _BV(MUX1) | _BV(MUX2) | _BV(MUX3) | _BV(MUX4));
	
	mode++;
	mode %= 2;
	
	if(mode == 0)
	{
		ADMUX |= _BV(MUX3) | _BV(MUX0); // x10, +:ADC1, -:ADC0
	}
	else
	{
		ADMUX |= _BV(MUX4); //x1, +:ADC0, -:ADC1
	}
}

/* interrupt when timer1 overflows */
ISR(TIMER1_OVF_vect)
{
	/* get the sample */
	uint16_t data;
	adc_get_sample(&data);

	if(data & 0x200)
		data |= 0xFC00;	
	
	uart_transmit(data >> 8);
	uart_transmit(data);	
	
	// uint8_t data;
	// adc_get_sample(&data);
	// uart_transmit(data);
	
	TCNT1 = 0xFF06;	
}
 