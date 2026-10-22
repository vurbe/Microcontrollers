///////////////

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "Configuration.h"
#define _XTAL_FREQ 640000000

volatile uint16_t adc_result = 0;
volatile uint8_t adc_complete = 0;


/*
 * ADC interrupt service routine
 */
void __interrupt() ADC_ISR(void)
{
    if (PIR1bits.ADIF && PIE1bits.ADIE) {

        // Right-justified 10-bit ADC result
        adc_result = ((uint16_t)ADRESH << 8) | ADRESL;

        // Clear ADC interrupt flag
        PIR1bits.ADIF = 0;

        // Tell main() that a conversion is complete
        adc_complete = 1;
    }
}


void oscillator_initialize(void)
{
    /*
     * Internal oscillator = 8 MHz
     *
     * IRCF = 110 selects 8 MHz on the PIC18F46K22.
     * PLL disabled.
     */
    OSCCON = 0b01100000;
    OSCTUNEbits.PLLEN = 0;
}


void port_initialize(void)
{
    /*
     * RA0 = AN0 analog input
     * RA1 and RA2 = output for the two upper ADC bits
     * Remaining PORTA pins = inputs
     */
    ANSELA = 0b00000001;
    TRISA  = 0b11111001;

    // PORTB = lower eight ADC result bits
    ANSELB = 0x00;
    TRISB  = 0x00;

    // PORTD configured as output and cleared
    ANSELD = 0x00;
    TRISD  = 0x00;

    LATB = 0x00;
    LATD = 0x00;
    LATA = 0x00;
}


void adc_initialize(void)
{
    /*
     * ADCON0:
     *
     * CHS = 0000  -> Select AN0
     * GO  = 0     -> Do not start conversion yet
     * ADON = 1    -> Enable ADC
     */
    ADCON0 = 0b00000001;

    /*
     * ADCON1:
     *
     * PVCFG = 00 -> Positive reference = VDD
     * NVCFG = 00 -> Negative reference = VSS
     */
    ADCON1 = 0b00000000;

    /*
     * ADCON2:
     *
     * ADFM = 1    -> Right-justified result
     * ACQT = 100   -> 8 TAD acquisition time
     * ADCS = 010   -> FOSC/32 ADC clock
     */
    ADCON2 = 0b10010010;

    // Clear ADC interrupt flag
    PIR1bits.ADIF = 0;

    // Enable ADC interrupt
    PIE1bits.ADIE = 1;

    // Enable peripheral interrupts
    INTCONbits.PEIE = 1;

    // Enable global interrupts
    INTCONbits.GIE = 1;
}


void main(void)
{
    oscillator_initialize();
    port_initialize();
    adc_initialize();

    while (1) {

        // Start ADC conversion
        ADCON0bits.GO = 1;

        // Wait until the ADC ISR reports completion
        while (adc_complete == 0) {
            ;
        }

        // Allow the next conversion
        adc_complete = 0;

        /*
         * Right-justified result format:
         *
         * ADRESL = ADC bits 7:0
         * ADRESH bits 1:0 = ADC bits 9:8
         */

        // Display lower eight bits on PORTB
        LATB = (uint8_t)(adc_result & 0x00FF);

        // Display upper two bits on RA1 and RA2
        LATA = (uint8_t)((adc_result >> 8) & 0x03);

        // Wait 100 ms before repeating
        __delay_ms(100);
    }
}
