/*
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include "uart.h"

#define STATUS_LED      PB0
#define STATUS_LED_PORT PORTB
#define STATUS_LED_DDR  DDRB

#define SET_LED()    (STATUS_LED_PORT |= (1<<STATUS_LED))
#define CLEAR_LED()  (STATUS_LED_PORT &= ~(1<<STATUS_LED))
#define TOGGLE_LED()    (STATUS_LED_PORT ^= (1<<STATUS_LED))

//volatile unsigned long micros;
volatile unsigned long millis;
volatile unsigned int sensors[6];// = {1545,235,15214,1453,2345,1554};
volatile unsigned char ADC_flag = 1;

void init()
{
    //set LED pin as output and set it high
    STATUS_LED_DDR |= (1<<STATUS_LED);
    STATUS_LED_PORT |= (1<<STATUS_LED);

    //UART initialization
    uart_init(UART_BAUD_SELECT(9600,F_CPU));

    //TIMER2 initialization ... CTC 1 ms, Prescaler 64
    TCCR2   = (1<<WGM21)|(0<<WGM20)|(1<<CS22)|(0<<CS21)|(0<<CS20); // normal CTC, OC2 disconnected, Prescaler CLK/64
    OCR2    = 0xF9; // for 1 ms interrput
    TIMSK   = (1<<OCIE2);

    //ADC initialization

    ADMUX = (0<<REFS1)|(1<<REFS0)|(0<<MUX3)|(0<<MUX2)|(0<<MUX1)|(0<<MUX0); // AVCC, Channel 0
    ADCSRA = (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); // En, Int. En.,  Division factor = 128 for 125KHz
    ADCSRA |= (1<<ADEN)|(1<<ADIE);
}

void send_sensors()
{
    unsigned char i = 0;
    char temp_str[16] = "0";
    for (i = 0;i < 6 ;i++ )
    {
        utoa(i,temp_str,10);
        uart_puts(temp_str);
        uart_puts_p(PSTR(": "));
        //utoa((sensors[i]*5)/1024.0,temp_str,10);
        dtostrf((sensors[i]*5)/1024.0,1,5,temp_str);
        uart_puts(temp_str);
        uart_puts_p(PSTR("\n\r"));
    }

    uart_puts_p(PSTR("\n\r"));
}

void send_sensor(unsigned char i)
{
    char temp_str[16] = "0";
    utoa(i,temp_str,10);
    uart_puts(temp_str);
    uart_puts_p(PSTR(": "));
    utoa(sensors[i],temp_str,10);
    uart_puts(temp_str);
    uart_puts_p(PSTR("\n\r\n\r"));
}

int main(void)
{

    char millis_str[16] = "0";
    char temp_str[16] = "0";
    unsigned long delay = 1000;
    unsigned long start_millis;
    unsigned char i = 0;

    init();

    sei(); // set global interrupt

    ADCSRA |= (1<<ADSC);
    uart_puts_p(PSTR("Hello World\n\r"));

    //_delay_ms(1000);

    start_millis = millis;

    ADMUX = (ADMUX&0xF0)|(0x04);

    for(;;)
    {
        if( (millis - start_millis) == delay)
        {
//            uart_puts_P("millis = ");
//            utoa(millis,millis_str,10);
//            uart_puts(millis_str);
//            uart_puts_P("\n\r");


            send_sensors();

            //send_sensor(0);


            start_millis = millis;
            //TOGGLE_LED();
            ADC_flag = 1;
            ADCSRA |= (1<<ADSC);
            CLEAR_LED();

        }
    }

    return 0;
}

ISR(ADC_vect)
{
    static unsigned char i = 0;
    unsigned char temp;

    if(ADC_flag == 1)
    {
        sensors[i++] = ADC;
        if(i > 5)
        {
            i = 0;
            ADC_flag = 0;
        }
        ADMUX = (ADMUX&0xF0)|i;
        ADCSRA |= (1<<ADSC);
    }




//    if(ADC_flag == 1)
//    {
//        sensors[0] = ADC;
//        ADC_flag = 0;
//    }

    SET_LED();

}

ISR(TIMER2_COMP_vect)
{
    //micros++;
    millis++;
//    if(millis == 1000)
//    {
//        TOGGLE_LED();
//        millis = 0;
//    }

}


