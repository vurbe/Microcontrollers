#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "Configuration.h"
#define _XTAL_FREQ 64000000UL



uint8_t increment(uint8_t count){
    static const uint8_t digits[10]= {
        0x3F,  // 0, add ~ in front of 0x3F to turn anode so 
        0x06,  // 1, ~0x3F (need to do for the whole list)
        0x5B,  // 2,        
        0x4F,  // 3,
        0x66,  // 4,
        0x6D,  // 5,
        0x7D,  // 6,
        0x07,  // 7,
        0x7F,  // 8,
        0x6F,  // 9,
    }
    return digits[count];
}

void main(void) {
    // Configure Oscillator 64MHz
    OSCCON = 0b01110000;
    OSCTUNE = 0b01000000;
    
    
    // Configure LCD Ports (PORTA & PORTD)
    ANSELB = 0x00;        // CRITICAL: Disable analog on PORTB for LCD data lines (RA4-RA7)
    ANSELC = 0x00;
    TRISB = 0x00;         // Set PORTB as outputs
    TRISC = 0x00;         // Set PortC as outputs
    TRISD = 0x00;         // Set PORTD as outputs

    LATB = 0x00;// Set PORTB as 0
    LATC = 0x00;// Set PortC as 0
    LATD = 0x00;// Set PORTD as 0

    int i = 0; // Initalize I,j,K (there is no k in this house)
    int j = 0; // so redundant I should delete but I dont wanna
    int k = 0;

    while (1){
        for(j=0;j<10;j++) {
            LATC = increment(j); // set tens
            for(i=0;i<10;i++){
                LATB = increment(i); //set ones
                __delay_ms(300);
                }

        }
    }
}
