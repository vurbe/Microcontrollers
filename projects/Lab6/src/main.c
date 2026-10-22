///////////////

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "Configuration.h"

#define _XTAL_FREQ 4000000

void main() {
    TRISAbits.TRISA0=1;
    ANSELAbits.ANSA0=1;

    TRISB=0;
    TRISD=0;
    PORTB=0;
    PORTD=0;

    ADCON0=0b00000000;
    ADCON1=0b00000011;
    ADCON2=0b10100010;

    while(1){
        __delay_ms(100);
        ADCON0bits.GO =1;
        while(ADCON0bits.DONE==1);
        PORTB=ADRESL;
        PORTD=ADRESH;
        __delay_ms(250);
    }
}
