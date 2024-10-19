/*
 * @file main.c
 *
 * @brief Main source code for the Stopwatch_Design program.
 *
 * This file contains the main entry point and function definitions for the Stopwatch_Design program.
 * This lab involves designing a basic stopwatch. It demonstrates edge-triggered interrupts, 
 * and it interfaces with the following:
 *  - User LED (RGB) Tiva C Series TM4C123G LaunchPad
 *	- EduBase Board LEDs (LED0 - LED3)
 *	- EduBase Board Push Buttons (SW2 - SW3)
 *	- EduBase Board Seven-Segment Display
 *	- PMOD BTN module
 *
 * It configures the pins used by the EduBase Board push buttons (SW2 and SW3) and the PMOD
 * BTN module to generate interrupts on rising edges.
 * 
 * Timer 0A is used to generate periodic interrupts every 1 millisecond. The values of the 
 * stopwatch (milliseconds, seconds, and minutes) will increment in the Timer 0A
 * periodic task. The PMOD BTN module will be used to control the stopwatch.
 *
 * @author Frank Ramirez
 */

#include "TM4C123GH6PM.h"
#include "GPIO.h"
#include "Buzzer.h"
#include "Timer_0A_Interrupt.h"
#include "Seven_Segment_Display.h"
#include "EduBase_Button_Interrupt.h"
#include "PMOD_BTN_Interrupt.h"

//
void PMOD_BTN_Handler(uint8_t pmod_btn_status);

//
void EduBase_Button_Handler(uint8_t edubase_button_status);

//
static uint8_t counter = 0;

// Declare the function prototype for the function that calculates
// stopwatch value and stores it in an array
void Calculate_Stopwatch_Value(uint8_t stopwatch_value[]);

// Declare the function prototype for the user-defined function for Timer 0A
void Timer_0A_Periodic_Task(void);

// Initialize a global variable for Timer 0A to keep track of elapsed time in milliseconds
static uint8_t ms_elapsed = 0;

// Initialize global variables to keep track of the stopwatch value (i.e. milliseconds, seconds, and minutes)
// "milliseconds" is updated every 100 ms (Range: 0 to 999 ms)
// "seconds" is updated every 1000 ms (Range: 0 to 59 s)
// "minutes" is updated every 60 seconds (Range: 0 to 9 minutes)
static uint8_t milliseconds = 0;
static uint8_t seconds = 0;
static uint8_t minutes = 0;

// Initialize global flags for starting and resetting the stopwatch
static uint8_t start_stopwatch = 0;
static uint8_t reset_stopwatch = 0;

int main(void)
{
	//Initialize the push buttons on the PMOD BTN module (Port A)
	PMOD_BTN_Interrupt_Init(&PMOD_BTN_Handler);
	EduBase_Button_Init();
	Seven_Segment_Display_Init();
	EduBase_Button_Interrupt_Init(&EduBase_Button_Handler);
	RGB_LED_Init();
	Timer_0A_Interrupt_Init(&Timer_0A_Periodic_Task);
	uint8_t stopwatch_value[4] = {0};
	
	while(1)
	{
	EduBase_LEDs_Output(counter);
	Seven_Segment_Display(counter);
		
//	Calculate_Stopwatch_Value(stopwatch_value);
//	Seven_Segment_Display_Stopwatch(stopwatch_value);
	}
}

void PMOD_BTN_Handler(uint8_t pmod_btn_status)
{
    switch (pmod_btn_status)
    {
        // BTN0 (PA2) is pressed
        case 0x04:
        {
            break;
        }
        
        // BTN1 (PA3) is pressed
        case 0x08:
        {
            break;
        }
        
        // BTN2 (PA4) is pressed
        case 0x10:
        {
            break;
        }
        
        // BTN3 (PA5) is pressed
        case 0x20:
        {
            GPIOB->DATA = GPIOB->DATA ^ 0x08;
            break;
        }
        
        default:
        {
            break;
        }
    }
}


//void PMOD_BTN_Handler(uint8_t pmod_btn_status)
//{
//    switch (pmod_btn_status)
//    {
//        // BTN0 (PA2) is pressed
//        case 0x04:
//        {
//            if (counter >= 15)
//            {
//                counter = 0;
//            }
//            else
//            {
//                counter = counter + 1;
//            }
//            break;
//        }
//        
//        // BTN1 (PA3) is pressed
//        case 0x08:
//        {
//            if (counter <= 0)
//            {
//                counter = 15;
//            }
//            else
//            {
//                counter = counter - 1;
//            }
//            break;
//        }
//        
//        // BTN2 (PA4) is pressed
//        case 0x10:
//        {
//            counter = 0;
//            break;
//        }
//        
//        // BTN3 (PA5) is pressed
//        case 0x20:
//        {
//            break;
//        }
//        
//        default:
//        {
//            break;
//        }
//    }
// }

//void PMOD_BTN_Handler(uint8_t pmod_btn_status)
//{
//    switch(pmod_btn_status)
//    {
//        // BTN0 (PA2) is pressed
//        case 0x04:
//        {
//            RGB_LED_Output(RGB_LED_GREEN);
//            start_stopwatch = 0x01;
//            break;
//        }

//        // BTN1 (PA3) is pressed
//        case 0x08:
//        {
//            RGB_LED_Output(RGB_LED_RED);
//            start_stopwatch = 0x00;
//            break;
//        }

//        // BTN2 (PA4) is pressed
//        case 0x10:
//        {
//            RGB_LED_Output(RGB_LED_OFF);
//            reset_stopwatch = 0x01;
//            break;
//        }

//        // BTN3 (PA5) is pressed
//        case 0x20:
//        {
//            break;
//        }

//        default:
//        {
//            break;
//        }
//    }
//}

void EduBase_Button_Handler(uint8_t edubase_button_status)
{
    switch(edubase_button_status)
    {
        case 0x08:
        {
            if (counter >= 15)
            {
                counter = 0;
            }
            else
            {
                counter = counter + 1;
            }
            break;
        }

        case 0x04:
        {
            if (counter <= 0)
            {
                counter = 15;
            }
            else
            {
                counter = counter - 1;
            }
            break;
        }

        default:
        {
            break;
        }
    }
}

void Calculate_Stopwatch_Value(uint8_t stopwatch_value[])
{
    // Store the "milliseconds" value in the first index of the array
    stopwatch_value[0] = milliseconds;

    // Store the least significant digit of the "seconds" value
    // in the second index of the array
    stopwatch_value[1] = seconds % 10;

    // Store the most significant digit of the "seconds" value
    // in the third index of the array
    stopwatch_value[2] = seconds / 10;

    // Store the "minutes" value in the fourth index of the array
    stopwatch_value[3] = minutes;
}

void Timer_0A_Periodic_Task(void)
{
    if (start_stopwatch == 0x01)
    {
        ms_elapsed++;
        // Check if 100 milliseconds have passed
        if(ms_elapsed > 99){
            ms_elapsed = 0;  // Reset ms_elapsed
            milliseconds++;  // Increment milliseconds by 1
        }

        // Check if 1 second has passed
        if(milliseconds > 9){
            milliseconds = 0;  // Reset milliseconds
            seconds++;  // Increment seconds by 1
        }

        // Check if 60 seconds (1 minute) have passed
        if(seconds > 59){
            seconds = 0;  // Reset seconds
            minutes++;  // Increment minutes by 1
        }

        // Check if 10 minutes have passed
        if(minutes > 9){
            minutes = 0;  // Reset minutes
        }
    }

    if (reset_stopwatch == 0x01)
    {
        reset_stopwatch = 0x00;
        start_stopwatch = 0x00;
        ms_elapsed = 0;
        milliseconds = 0;
        seconds = 0;
        minutes = 0;
    }
}

