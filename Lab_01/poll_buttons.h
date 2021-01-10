// Helper functions for polling buttons on the joystick

#ifndef POLL_BUTTONS_H
#define POLL_BUTTONS_H

// Each function checks if the according button is pressed
// by reading the corresponding GPIO and sets, resets, or
// toggles the according LED

/* 
Button pins:
	 Center = PA0
   Left = PA1
   Right = PA2
   Up = PA3
   Down = PA5
	 
LED pins:
	 Red = PB2
	 Green = PE8
*/

// Toggle both LEDs
void center(void);

// Toggle green LED
void left(void);

// Toggle red LED
void right(void);

// Set both LEDs
void up(void);

// Reset both LEDs
void down(void);

#endif
