/**
 * @file EduBase_Button_Interrupt.c
 *
 * @brief Source code for the EduBase_Button_Interrupt driver.
 *
 * This file contains the function definitions for the EduBase_Button_Interrupt driver.
 * It interfaces with the EduBase Board push buttons. The following pins are used:
 *	- SW2 (PD3)
 *	- SW3 (PD2)
 *
 * It configures the pins to trigger interrupts on rising edges. The EduBase Board 
 * push buttons operate in an active high configuration.
 *
 * @author Aaron Nanas
 */

#include "EduBase_Button_Interrupt.h"

// Declare a pointer to the user-defined task
void (*EduBase_Button_Task)(uint8_t edubase_button_status);

void EduBase_Button_Interrupt_Init(void(*task)(uint8_t))
{
	// Store the user-defined task function for use during interrupt handling
	EduBase_Button_Task = task;
	
	// Enable the clock to Port D by setting the R3 bit (Bit 3) in the RCGCGPIO register
	SYSCTL->RCGCGPIO |= 0x08;
	
	// Configure the PD3 and PD2 pins as input by clearing Bits 3 to 2 in the DIR register
	GPIOD->DIR &= ~0x0C;
	
	// Configure the PD3 and PD2 pins to function as
	// GPIO pins by clearing Bits 3 to 2 in the AFSEL register
	GPIOD->AFSEL &= ~0x0C;
	
	// Enable the digital functionality for the PD3 and PD2 pins
	// by setting Bits 3 to 2 in the DEN register
	GPIOD ->DEN |= 0x0C;
	
	// Enable the weak pull-down resistor for the PD3 and PD2 pins
	// by setting Bits 3 to 2 in the PDR register
	GPIOD->PDR |= 0x0C;
	
	// Configure the PD3 and PD2 pins to detect edges
	// by clearing Bits 3 to 2 in the IS register
	GPIOD ->IS &= ~0x0C;
	
	// Allow the GPIOIEV register to handle interrupt generation
	// and determine which edge to check for the PD3 and PD2 pins 
	// by clearing Bits 3 to 2 in the IBE register
	GPIOD ->IBE &= ~0x0C;
	
	// Configure the PD3 and PD2 pins to detect
	// rising edges by setting Bits 3 to 2 in the IEV register
	// Rising edges on the corresponding pins will trigger interrupts
	GPIOD->IEV |= 0x0C;
	
	// Clear any existing interrupt flags on the PD3 and PD2 pins
	// by setting Bits 3 to 2 in the ICR register
	GPIOD->ICR |= 0x0C;
	
	// Allow the interrupts that are generated by the PD3 and PD2 pins to be 
	// sent to the interrupt controller by setting Bits 3 to 2 in the IM register
	GPIOD ->IM |= 0x0C;
	
	// Clear the INTD field (Bits 31 to 29) of the IPR[0] register (PRI0)
	NVIC->IPR[0] &= ~0xE00000000;
	
	// Set the priority level of the interrupts to 3. Port D has an Interrupt Request (IRQ) number of 3
	NVIC->IPR[0] |= (3 << 29);
	
	// Enable IRQ 3 for GPIO Port D by setting Bit 3 in the ISER[0] register
	NVIC->ISER[0] |= (1 << 3);
}

uint8_t PMOD_BTN_Read(void)
{
	//Declare a local variable to store the status of the PMOD BTN
	//Then, read the DATA register for Port A
	//A "0x3C" bit mask is ysed to capture only the pins used in the PMOD BTN
	uint8_t pmod_btn_state = GPIOA->DATA & 0x3C;
	
	//Return the status of the PMOD BTN module
	return pmod_btn_state;
}

void GPIOD_Handler(void)
{
	//Check if an interrupt has been triggered by any of
	//the following pins: PD2 and PD3
	if (GPIOD->MIS & 0x0C)
	{
		//Execute the used-defined function and pass the 
		//status of the EduBase board push buttons
		(*EduBase_Button_Task)(Get_EduBase_Button_Status());
		//Acknowledge the interrupt from any of the following pins
		// and clear it: PD2 and PD3
		GPIOD->ICR |= 0x0C;
	}
}


