///////////////

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "Configuration.h"
#include <stdbool.h>
#include "LCD.h"

#define _XTAL_FREQ 4000000

#define TARGET_DISTANCE_CM 50u
#define TARGET_OUTPUT_LAT  LATBbits.LATB2

uint8_t button_pressed(void) { //Thank you lab4 me. 
    static uint8_t last_state = 1; // Idle state is 1 (HIGH)
    uint8_t current_state = PORTBbits.RB4;

    // Detect transition from released (1) to pressed (0)
    if (last_state == 1 && current_state == 0) {
        __delay_ms(10); // Debounce delay 10 for lower _XTAL_FREQ
        
        // Re-verify that button is still held down
        if (PORTBbits.RB4 == 0) {
            last_state = 0; // Lock state so holding down doesn't re-trigger
            return 1;       // Trigger event ONLY on initial press
        }
    } 
    // Reset state ONLY when button is released back to HIGH
    else if (current_state == 1) {
        last_state = 1;
    }

    return 0; // No new press event
}

void update_display(bool status, uint32_t distance) { //Refactor from Lab4 (im lazy)
    // Update Line 1: count=XX
    LCD_cursor_set(1, 1);
    if(status){
        LCD_write_string("Target Detected??");
    }
    else{
        LCD_write_string("   "); // Clear trailing numbers when shifting digits
    }
    // Update Line 2: round=XX
    LCD_cursor_set(2, 1);

    LCD_write_string("Distance(cm)=");
    LCD_write_variable(distance, 0);
    LCD_write_string("   "); // Clear trailing numbers when shifting digits
}

static void timer1_init(void)
{
    T1CON = 0x00;

    /*
     * Internal clock source: FOSC/4
     * Timer1 prescaler: 1:8
     */
    T1CONbits.TMR1CS = 0b00;
    T1CONbits.T1CKPS = 0b11;
    T1CONbits.T1OSCEN = 0;
    T1CONbits.RD16 = 1;

    TMR1H = 0x00;
    TMR1L = 0x00;

    PIR1bits.TMR1IF = 0;
    T1CONbits.TMR1ON = 0;
}

static bool measure_distance_cm(uint16_t *distance_cm)
{
    uint16_t wait_time_us = 0u;
    uint16_t timer_counts;
    uint32_t time_us;

    /*
     * Make sure the sensor has stabilized.
     */
    LATBbits.LATB0 = 0;
    __delay_ms(10);

    /*
     * Generate the required approximately 10-us trigger pulse.
     */
    LATBbits.LATB0 = 1;
    __delay_us(10);
    LATBbits.LATB0 = 0;

    /*
     * Wait for ECHO to become high.
     * The timeout prevents the program from hanging if no echo arrives.
     */
    while ((PORTBbits.RB1 == 0u) && (wait_time_us < 60000u)) {
        __delay_us(1);
        wait_time_us++;
    }

    /*
     * No echo received.
     */
    if (PORTBbits.RB1 == 0u) {
        return false;
    }

    /*
     * Start Timer1 when ECHO becomes high.
     */
    TMR1H = 0x00;
    TMR1L = 0x00;
    PIR1bits.TMR1IF = 0;
    T1CONbits.TMR1ON = 1;

    /*
     * ECHO remains high while the ultrasonic pulse is traveling.
     */
    while ((PORTBbits.RB1 == 1u) &&
           (PIR1bits.TMR1IF == 0u)) {
        /* Wait */
    }

    T1CONbits.TMR1ON = 0;

    /*
     * Timer overflow means the echo lasted too long.
     */
    if (PIR1bits.TMR1IF != 0u) {
        PIR1bits.TMR1IF = 0;
        return false;
    }

    /*
     * Read Timer1.
     *
     * At FOSC = 4 MHz:
     * FOSC/4 = 1 MHz
     * Timer1 tick with 1:8 prescaler = 8 us
     */
    uint8_t timer_low;
    uint8_t timer_high;

    timer_low = TMR1L;
    timer_high = TMR1H;

    timer_counts = ((uint16_t)timer_high << 8) | timer_low;    time_us = (uint32_t)timer_counts * 8UL;

    /*
     * Ultrasonic distance:
     *
     * distance in cm = echo time in microseconds / 58
     */
    *distance_cm = (uint16_t)(time_us / 58UL);

    return true;
}


void main(void) {

    uint16_t distance_cm = 0u;
    bool target_detected = false;
    bool valid_measurement = false;
    // Configure Button Input (RB4 on Curiosity HPC)
    TRISBbits.TRISB4 = 1;
    ANSELB = 0x00; // Digital input for B port for maximizing lazy

    // Ports for the HC-SRO4
    TRISBbits.TRISB0 = 0; //RB0 output (Trig pin)
    TRISBbits.TRISB1 = 1; //RB1 input (Echo pin)
    TRISBbits.TRISB2 = 0;   // LED/buzzer output
    
    // Configure LCD Ports (PORTA & PORTD)
    ANSELA = 0x00;        // CRITICAL: Disable analog on PORTA for LCD data lines (RA4-RA7)
    TRISC = 0x00;         // Set PORTC as outputs
    TRISD = 0x00;         // Set PORTD as outputs

    LATB = 0x00;
    LATC = 0x00;
    LATD = 0x00;

    // Initialize LCD
    LCD_init();
    //initialize clock
    timer1_init();

    // Display baseline values
    update_display(false, 0u);

    while (1) {
        
        // Execute increment only on new button press event
        if (button_pressed()) {
            distance_cm=0u;
            target_detected = false;
            TARGET_OUTPUT_LAT = 0;
            update_display(false, 0u);
        }

                valid_measurement = measure_distance_cm(&distance_cm);

        /*
         * A target is detected only when:
         * 1. The measurement was valid, and
         * 2. The distance is 50 cm or less.
         */
        target_detected =
            valid_measurement &&
            (distance_cm <= TARGET_DISTANCE_CM);

        update_display(target_detected, distance_cm);

        /*
         * Turn on the LED/buzzer for one second if a target is detected.
         */
        if (target_detected) {
            TARGET_OUTPUT_LAT = 1;
            __delay_ms(1000);
            TARGET_OUTPUT_LAT = 0;
        }
        else {
            TARGET_OUTPUT_LAT = 0;
            __delay_ms(100);
        }
    }
}
