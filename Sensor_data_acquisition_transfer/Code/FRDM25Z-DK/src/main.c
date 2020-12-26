/*----------------------------------------------------------------------------
 *      
 *----------------------------------------------------------------------------
 *      Name:    main.C
 *      Purpose: sensor data aquisition and transmission 
 *----------------------------------------------------------------------------
 *      
 *      
 *---------------------------------------------------------------------------*/
#include <MKL25Z4.H>  				// Device header
#include <stdio.h>
#include <math.h>
#include "gpio_defs.h"
#include "LEDs.h"
#include "i2c.h"
#include "mma8451.h"
#include "delay.h"
#include "UART.h" 

unsigned int switch_Val = 0;
/*----------------------------------------------------------------------------
 *         Function 'SysTick_Handler': sys tick called every 0.25 sec
 *---------------------------------------------------------------------------*/

void SysTick_Handler(){
	//printf("\n\r xvalue=%d,yvalue=%d,zvalue=%d roll=%f pitch=%f\n\r",acc_X,acc_Y,acc_Z, roll, pitch );
	if(switch_Val == 1)
	{
	Control_RGB_LEDs(0, 0, 1);
	UART_data_packet();
	printf("\n\r start_code= %d Xlsb = %d Xmsb = %d Ylsb = %d Ymsb = %d Zlsb = %d Zmsb = %d checksum = %d\n\r",xyz_data[0],xyz_data[1],xyz_data[2],xyz_data[3],xyz_data[4],xyz_data[5],xyz_data[6],xyz_data[7] );
	Delay(10); //11/29
	UART0_Transmit_Poll_xyz(xyz_data);
	Control_RGB_LEDs(0, 0, 0);
}
}

/*--------------------------------------------------------------------------------
 *         Function 'init_SysTick': To initialize system timer with input as ticks
 *--------------------------------------------------------------------------------*/

uint32_t init_SysTick(uint32_t ticks)
{
  if ((ticks - 1UL) > SysTick_LOAD_RELOAD_Msk)
  {
    return (1UL);                                                   /* Reload value impossible */
  }

  SysTick->LOAD  = (uint32_t)(ticks - 1UL);                         /* set reload register */
  NVIC_SetPriority (SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL); /* set Priority for Systick Interrupt */
  SysTick->VAL   = 0UL;                                             /* Load the SysTick Counter Value */
  SysTick->CTRL  = SysTick_CTRL_TICKINT_Msk   |
                   SysTick_CTRL_ENABLE_Msk;                         /* Enable SysTick IRQ and SysTick Timer */
  return (0UL);                                                     /* Function successful */
}

/*----------------------------------------------------------------------------
  Function 'MAIN': MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {
	Init_UART0(115200); 													/* UART baudrate set to 115200 */ 
	Init_RGB_LEDs();
	i2c_init();																/* init i2c	*/
	init_SysTick(750000UL);
	if (!init_mma()) {												/* init mma peripheral */
		Control_RGB_LEDs(0, 1, 0);							/* Light green error LED */
		while (1)																/* not able to initialize mma */
			;
	}
	Control_RGB_LEDs(1, 1, 0);							
	Delay(100);
	printf("\n\r ECE 4721 Project: Accelerometer Sensor Data Acquisition\n\r");
	
	while (1) {														/* Infinite loop */
		switch_Val = (0x0001 & ((~PTB->PDIR)));
		read_full_xyz();
		convert_xyz_to_roll_pitch();
		// Light green LED if pitch > 10 degrees
		// Light blue LED if roll > 10 degrees
		Control_RGB_LEDs(0, (fabs(roll) > 10)? 1:0, (fabs(pitch) > 10)? 1:0);
	}
}

