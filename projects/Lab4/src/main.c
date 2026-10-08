#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "Configuration.h"
#include "LCD.h"

#define _XTAL_FREQ 64000000UL

/**
 * Debounce helper for Curiosity HPC Board (Active-Low S1 / RB4)
 * Triggers strictly on button press down (Falling Edge: HIGH -> LOW)
 */
uint8_t button_pressed(void) {
    static uint8_t last_state = 1; // Idle state is 1 (HIGH)
    uint8_t current_state = PORTBbits.RB4;

    // Detect transition from released (1) to pressed (0)
    if (last_state == 1 && current_state == 0) {
        __delay_ms(20); // Debounce delay
        
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

void update_display(uint8_t count, uint32_t round) {
    // Update Line 1: count=XX
    LCD_cursor_set(1, 1);
    LCD_write_string("count=");
    LCD_write_variable(count, 0);
    LCD_write_string("   "); // Clear trailing numbers when shifting digits

    // Update Line 2: round=XX
    LCD_cursor_set(2, 1);
    LCD_write_string("round=");
    LCD_write_variable(round, 0);
    LCD_write_string("   "); // Clear trailing numbers when shifting digits
}

void main(void) {
    // Configure Oscillator 64MHz
    OSCCON = 0b01110000;
    OSCTUNE = 0b01000000;
    
    // Configure Button Input (RB4 on Curiosity HPC)
    TRISBbits.TRISB4 = 1;
    ANSELBbits.ANSB4 = 0; // Digital input for RB4
    
    // Configure LCD Ports (PORTA & PORTD)
    ANSELA = 0x00;        // CRITICAL: Disable analog on PORTA for LCD data lines (RA4-RA7)
    TRISA = 0x00;         // Set PORTA as outputs
    TRISD = 0x00;         // Set PORTD as outputs

    LATA = 0x00;
    LATD = 0x00;

    // Initialize LCD
    LCD_init();

    uint8_t count = 0;
    uint32_t round = 0;

    // Display baseline values
    update_display(count, round);

    while (1) {
       
        count++;
        if(count>15){
            count = 0
            round++;
        }
        // Execute increment only on new button press event
        if (button_pressed()) {
            count=0;
            round = 0;
            // set button pressed to 0
            update_display(count, round);
        }
    }
}
