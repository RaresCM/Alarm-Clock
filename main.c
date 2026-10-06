#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

void TWIinit (void) {
    TWBR = 72; //Prescaler(100Khz);
    TWSR = 0x00;//Prescaler (100Khz)
    TWCR = (1 << TWEN);//TWI On
}

void TWIstop(void) {
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);// Waiting | STOP Condition Generated | TWI On
}

void TWIstart(void) {
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);// Waiting | Become Master | TWI On
    while(!(TWCR & (1 << TWINT))) {}//While we start wait
}

uint8_t TWIreadack(void) {
    TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN);// Done waiting | Send ACK | TWi On
    while(!(TWCR & (1 << TWINT))) {}//While we read the bytes and put in TWDR wait
    return TWDR;// Place bytes in TWDR
}

uint8_t TWIreadnack(void) {
    TWCR = (1 << TWINT) | (1 << TWEN);// Done waiting | TWi On
    while (!(TWCR & (1 << TWINT))) {}// Wait for data to be read and placed in TWDR
    return TWDR;//Place bytes in TWDR
}

void TWIwrite (uint8_t data) {
    TWDR = data; //Load TWDR with the bytes of data to send
    TWCR = (1 << TWINT) | (1 << TWEN);//Clear TWINT and execute the send | TWI On
    while(!(TWCR & (1 << TWINT))) {}//While data is sending wait
}

uint8_t numbertobcd(uint8_t value) {
    return (((value / 10) << 4) | (value % 10));
}

uint8_t bcdtonumber(uint8_t value) {
    return (((value >> 4) * 10) + (value & 0x0F));
}

void LCDsendcommand(uint8_t command) {
    PORTC = command;
    PORTE &= ~(1 << PE4); //RS Low to indicate command
    PORTE |= (1 << PE5);// Latch High to let it read PORTC = command
    _delay_us(1);//1 microsecond for system to stabalise
    PORTE &= ~(1 << PE5);//Latch low to lock in command
    _delay_ms(2);//2 miliseconds to exectue command
}

void LCDsenddata(uint8_t data) {
    PORTC = data;
    PORTE |= (1 << PE4);//RS High to indicate data
    PORTE |= (1 << PE5);//Latch high to let it read PORTC = data
    _delay_us(1);//1 microsecond for system to stabalise
    PORTE &= ~(1 << PE5);//Latch low to lock data
    _delay_ms(2);//2 milisecond delay to execute data
}

void LCDinit(void) {
    DDRC = 0b11111111; //Set Data memory address to output
    DDRE |= (1 << DDE4) | (1 << DDE5);// D2 RS Pin | D3 Latch Pin
    _delay_ms(20);//20 milisecond delay for power rails to stabalise
    LCDsendcommand(0x30);//Wake up/Reset call
    _delay_ms(3);//3 ms plus 2ms from LCD sendcommand equal 5 ms > 4.1ms recommended for wakeup/reset
    LCDsendcommand(0x38);//Function set 8 bit data, 2 rows, 5x8 pixels
    LCDsendcommand(0x01);//Clear screen and memory
    LCDsendcommand(0x0F);//Display ON with blinking box
}

void LCDsendstring(char *str) {//Function to show text on LCD
    while(*str) {
        LCDsenddata(*str++);
    }
}

int main(void){
    TWIinit();
    LCDinit();
    DDRG |= (1 << DDG5); //D3 Buzzer out
    PORTG &= ~(1 << PG5);
    DDRL &= ~((1 << DDL7) | (1 << DDL6) | (1 << DDL5) | (1 << DDL4) | (1 << DDL3));//D42 Alarm silence | D43 Right| D44 Decrease | D45 Increase| D46 Left
    PORTL |= (1 << PL7) | (1 << PL6) | (1 << PL5) | (1 << PL4) | (1 << PL3);
    uint8_t sec, min, hour, date, month, year, ahour, amin, asec;
    uint8_t editindex = 0;
    TCCR1A = 0x00;
    TCCR1B = (1 << CS12) | (1 << CS10);

    uint8_t digitselect[6] = {0xC8, 0xC9, 0xCB, 0xCC, 0xCE, 0xCF};
    uint8_t alarmdigit[6] = {0, 0, 0, 0, 0, 0};
    char timebuffer[16];
    char datebuffer[16];
    char alarmtext[8];
    char alarmtime[16];

    while(1) {
    TWIstart();
    TWIwrite(0xD0); // Clock Module wake up
    TWIwrite(0x00); //Go to seconds memeory register
    TWIstart(); //Keep control of TWI line
    TWIwrite(0xD1); //Change last bit to 1 from 1 to start reading

    sec = bcdtonumber(TWIreadack());
    min = bcdtonumber(TWIreadack());
    hour = bcdtonumber(TWIreadack());
    TWIreadack();
    date = bcdtonumber(TWIreadack());
    month = bcdtonumber(TWIreadack());
    year = bcdtonumber(TWIreadnack());

    TWIstop();

    if(!(PINL & (1 << PL3))) {//Left cursor move for alarm setting
        TCNT1 = 0; TIFR1 |= (1 << TOV1);
        if(editindex > 0) {
            editindex--;
        } else {
            editindex = 5;
        }
    }

    if(!(PINL & (1 << PL6))) {//Right cursor move for alarm setting
        TCNT1 = 0; TIFR1 |= (1 << TOV1);
        if(editindex < 5) {
            editindex++;
        } else {
            editindex = 0;
        }
    }

    if(!(PINL & (1 << PL4))) {//Increase the number in the selected alarm digit
        TCNT1 = 0; TIFR1 |= (1 << TOV1);
        alarmdigit[editindex]++;

        if (editindex == 0 && alarmdigit[0] > 2) {
            alarmdigit[0] = 0; // Hours tens max is 2
        } else if ((editindex == 2 || editindex == 4) && alarmdigit[editindex] > 5) {
            alarmdigit[editindex] = 0; // Minutes/Seconds tens max is 5
        } else if (alarmdigit[editindex] > 9) {
            alarmdigit[editindex] = 0; // All ones-digits max is 9
        }
    }

    if(!(PINL & (1 << PL5))) {//Decrease the number in the selected alarm digit
        TCNT1 = 0; TIFR1 |= (1 << TOV1);
        alarmdigit[editindex]--;

        if (alarmdigit[editindex] == 255) {
            if (editindex == 0) {
                alarmdigit[0] = 2; // Wrap back to 2 for hours tens
            } else if (editindex == 2 || editindex == 4) {
                alarmdigit[editindex] = 5; // Wrap back to 5 for minutes/seconds tens
            } else {
                alarmdigit[editindex] = 9; // Wrap back to 9 for ones-digits
            }
        }
    }

    if(!(PINL & (1 << PL7))){//Alarm silence
        PORTG &= ~(1 << PG5);
    }

    ahour = ((alarmdigit[0] * 10) + alarmdigit[1]);
    amin = ((alarmdigit[2] * 10) + alarmdigit[3]);
    asec = ((alarmdigit[4] * 10) + alarmdigit[5]);

    if (TIFR1 & (1 << TOV1)) { //Turn off blidnking cursor if not used
        LCDsendcommand(0x0C);
    } else {
        LCDsendcommand(0x0F);
    }

    sprintf(timebuffer, "%02d:%02d:%02d", hour, min, sec);
    sprintf(datebuffer, "%02d/%02d/%02d", date, month, year);
    sprintf(alarmtext, "Alarm:");
    sprintf(alarmtime,"%02d:%02d:%02d", ahour, amin, asec);

    LCDsendcommand((0x80 + 9));
    LCDsendstring(alarmtext);
    LCDsendcommand(0x80);
    LCDsendstring(timebuffer);
    LCDsendcommand(0xC0);
    LCDsendstring(datebuffer);
    LCDsendcommand((0xC0 + 8));
    LCDsendstring(alarmtime);
    LCDsendcommand(digitselect[editindex]);

    if((ahour == hour) & (amin == min) & (asec == sec)) { //Alarm buzzer on if time matches
        PORTG |= (1 << PG5);
    }

    _delay_ms(500);

    }

    return 0;
}