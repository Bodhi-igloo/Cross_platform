/*----------------------------------------------------------------------------
 *      
 *----------------------------------------------------------------------------
 *      Name:    UART.C
 *      Purpose: sensor data aquisition and transmission 
 *----------------------------------------------------------------------------
 *      BEGIN - UART0 Device Driver
 *      
 *---------------------------------------------------------------------------*/

#include "UART.h"
#include <stdio.h>

//#define Debug_mode //11/30



/* BEGIN - UART0 Device Driver

*/
struct __FILE
{
  int handle;
};

FILE __stdout;  //Use with printf
FILE __stdin;		//use with fget/sscanf, or scanf

/*----------------------------------------------------------------------------
 *         Function 'fputc':Retarget the fputc method to use the UART0 
 *---------------------------------------------------------------------------*/

int fputc(int ch, FILE *f){
	while(!(UART0->S1 & UART_S1_TDRE_MASK) && !(UART0->S1 & UART_S1_TC_MASK));
	UART0->D = ch;
	return ch;
}

/*----------------------------------------------------------------------------
 *         Function 'fputc':Retarget the fgetc method to use the UART0 
 *---------------------------------------------------------------------------*/

int fgetc(FILE *f){
	while(!(UART0->S1 & UART_S1_RDRF_MASK));
	return UART0->D;
}
#if defined(Debug_mode)  // 11/29
// PTA 1,2 for terminal logging
//void Init_UART0(uint32_t baud_rate) {
//	uint16_t sbr;
//	uint8_t temp;
//	
//	// Enable clock gating for UART0 and Port A
//	SIM->SCGC4 |= SIM_SCGC4_UART0_MASK; 										
//	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;											
//	
//	// Make sure transmitter and receiver are disabled before init
//	UART0->C2 &= ~UART0_C2_TE_MASK & ~UART0_C2_RE_MASK; 		
//	
//	// Set UART clock to 48 MHz clock 
//	SIM->SOPT2 |= SIM_SOPT2_UART0SRC(1);
//	SIM->SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK;

//	// Set pins to UART0 Rx and Tx
//	PORTA->PCR[1] = PORT_PCR_ISF_MASK | PORT_PCR_MUX(2); // Rx 
//	PORTA->PCR[2] = PORT_PCR_ISF_MASK | PORT_PCR_MUX(2); // Tx 
//	
//	// Set baud rate and oversampling ratio
//	sbr = (uint16_t)((SYS_CLOCK)/(baud_rate * UART_OVERSAMPLE_RATE)); 			
//	UART0->BDH &= ~UART0_BDH_SBR_MASK;
//	UART0->BDH |= UART0_BDH_SBR(sbr>>8);
//	UART0->BDL = UART0_BDL_SBR(sbr);
//	UART0->C4 |= UART0_C4_OSR(UART_OVERSAMPLE_RATE-1);				

//	// Disable interrupts for RX active edge and LIN break detect, select one stop bit
//	UART0->BDH |= UART0_BDH_RXEDGIE(0) | UART0_BDH_SBNS(0) | UART0_BDH_LBKDIE(0);
//	
//	// Don't enable loopback mode, use 8 data bit mode, don't use parity
//	UART0->C1 = UART0_C1_LOOPS(0) | UART0_C1_M(0) | UART0_C1_PE(0); 
//	// Don't invert transmit data, don't enable interrupts for errors
//	UART0->C3 = UART0_C3_TXINV(0) | UART0_C3_ORIE(0)| UART0_C3_NEIE(0) 
//			| UART0_C3_FEIE(0) | UART0_C3_PEIE(0);

//	// Clear error flags
//	UART0->S1 = UART0_S1_OR(1) | UART0_S1_NF(1) | UART0_S1_FE(1) | UART0_S1_PF(1);

//	// Try it a different way
//	UART0->S1 |= UART0_S1_OR_MASK | UART0_S1_NF_MASK | 
//									UART0_S1_FE_MASK | UART0_S1_PF_MASK;
//	
//	// Send LSB first, do not invert received data
//	UART0->S2 = UART0_S2_MSBF(0) | UART0_S2_RXINV(0); 
//	
//#if USE_UART_INTERRUPTS
//	// Enable interrupts. Listing 8.11 on p. 234
//	Q_Init(&TxQ);
//	Q_Init(&RxQ);

//	NVIC_SetPriority(UART0_IRQn, 2); // 0, 1, 2, or 3
//	NVIC_ClearPendingIRQ(UART0_IRQn); 
//	NVIC_EnableIRQ(UART0_IRQn);

//	// Enable receive interrupts but not transmit interrupts yet
//	UART0->C2 |= UART_C2_RIE(1);
//#endif

//	// Enable UART receiver and transmitter
//	UART0->C2 |= UART0_C2_RE(1) | UART0_C2_TE(1);	
//	
//	// Clear the UART RDRF flag
//	temp = UART0->D;
//	UART0->S1 &= ~UART0_S1_RDRF_MASK;

//}

/*----------------------------------------------------------------------------
 *         Function 'Init_UART0': PTA 1, 2 
 *---------------------------------------------------------------------------*/

void Init_UART0(uint32_t baud_rate) {
	uint16_t sbr;
	uint8_t temp;
	
	// Enable clock gating for UART0 and Port A
	SIM->SCGC4 |= SIM_SCGC4_UART0_MASK; 										
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;											
	
	// Make sure transmitter and receiver are disabled before init
	UART0->C2 &= ~UART0_C2_TE_MASK & ~UART0_C2_RE_MASK; 		
	
	// Set UART clock to 48 MHz clock 
	SIM->SOPT2 |= SIM_SOPT2_UART0SRC(1);
	SIM->SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK;

	// Set pins to UART0 Rx and Tx
	PORTA->PCR[1] = PORT_PCR_ISF_MASK | PORT_PCR_MUX(2); // Rx 
	PORTA->PCR[2] = PORT_PCR_ISF_MASK | PORT_PCR_MUX(2); // Tx 
	
	// Set baud rate and oversampling ratio
	sbr = (uint16_t)((SYS_CLOCK)/(baud_rate * UART_OVERSAMPLE_RATE)); 			
	UART0->BDH &= ~UART0_BDH_SBR_MASK;
	UART0->BDH |= UART0_BDH_SBR(sbr>>8);
	UART0->BDL = UART0_BDL_SBR(sbr);
	UART0->C4 |= UART0_C4_OSR(UART_OVERSAMPLE_RATE-1);				

	// Disable interrupts for RX active edge and LIN break detect, select one stop bit
	UART0->BDH |= UART0_BDH_RXEDGIE(0) | UART0_BDH_SBNS(0) | UART0_BDH_LBKDIE(0);
	
	// Don't enable loopback mode, use 8 data bit mode, don't use parity
	UART0->C1 = UART0_C1_LOOPS(0) | UART0_C1_M(0) | UART0_C1_PE(0); 
	// Don't invert transmit data, don't enable interrupts for errors
	UART0->C3 = UART0_C3_TXINV(0) | UART0_C3_ORIE(0)| UART0_C3_NEIE(0) 
			| UART0_C3_FEIE(0) | UART0_C3_PEIE(0);

	// Clear error flags
	UART0->S1 = UART0_S1_OR(1) | UART0_S1_NF(1) | UART0_S1_FE(1) | UART0_S1_PF(1);

	// Try it a different way
	UART0->S1 |= UART0_S1_OR_MASK | UART0_S1_NF_MASK | 
									UART0_S1_FE_MASK | UART0_S1_PF_MASK;
	
	// Send LSB first, do not invert received data
	UART0->S2 = UART0_S2_MSBF(0) | UART0_S2_RXINV(0); 
	
#if USE_UART_INTERRUPTS
	// Enable interrupts. Listing 8.11 on p. 234
	Q_Init(&TxQ);
	Q_Init(&RxQ);

	NVIC_SetPriority(UART0_IRQn, 2); // 0, 1, 2, or 3
	NVIC_ClearPendingIRQ(UART0_IRQn); 
	NVIC_EnableIRQ(UART0_IRQn);

	// Enable receive interrupts but not transmit interrupts yet
	UART0->C2 |= UART_C2_RIE(1);
#endif

	// Enable UART receiver and transmitter
	UART0->C2 |= UART0_C2_RE(1) | UART0_C2_TE(1);	
	
	// Clear the UART RDRF flag
	temp = UART0->D;
	UART0->S1 &= ~UART0_S1_RDRF_MASK;

}

#else
/*----------------------------------------------------------------------------
*         Function 'Init_UART0': PTE 20 21 
*---------------------------------------------------------------------------*/

void Init_UART0(uint32_t baud_rate) {
	uint16_t sbr;
	uint8_t temp;
	
	// Enable clock gating for UART0 and Port E
	SIM->SCGC4 |= SIM_SCGC4_UART0_MASK; 										
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;											
	
	// Make sure transmitter and receiver are disabled before init
	UART0->C2 &= ~UART0_C2_TE_MASK & ~UART0_C2_RE_MASK; 		
	
	// Set UART clock to 48 MHz clock 
	SIM->SOPT2 |= SIM_SOPT2_UART0SRC(1);
	SIM->SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK;

	// Set pins to UART0 Rx and Tx
	PORTE->PCR[21] = PORT_PCR_ISF_MASK | PORT_PCR_MUX(4); // Rx 
	PORTE->PCR[20] = PORT_PCR_ISF_MASK | PORT_PCR_MUX(4); // Tx 
	
	// Set baud rate and oversampling ratio
	sbr = (uint16_t)((SYS_CLOCK)/(baud_rate * UART_OVERSAMPLE_RATE)); 			
	UART0->BDH &= ~UART0_BDH_SBR_MASK;
	UART0->BDH |= UART0_BDH_SBR(sbr>>8);
	UART0->BDL = UART0_BDL_SBR(sbr);
	UART0->C4 |= UART0_C4_OSR(UART_OVERSAMPLE_RATE-1);				

	// Disable interrupts for RX active edge and LIN break detect, select one stop bit
	UART0->BDH |= UART0_BDH_RXEDGIE(0) | UART0_BDH_SBNS(0) | UART0_BDH_LBKDIE(0);
	
	// Don't enable loopback mode, use 8 data bit mode, don't use parity
	UART0->C1 = UART0_C1_LOOPS(0) | UART0_C1_M(0) | UART0_C1_PE(0); 
	// Don't invert transmit data, don't enable interrupts for errors
	UART0->C3 = UART0_C3_TXINV(0) | UART0_C3_ORIE(0)| UART0_C3_NEIE(0) 
			| UART0_C3_FEIE(0) | UART0_C3_PEIE(0);

	// Clear error flags
	UART0->S1 = UART0_S1_OR(1) | UART0_S1_NF(1) | UART0_S1_FE(1) | UART0_S1_PF(1);

	// Try it a different way
	UART0->S1 |= UART0_S1_OR_MASK | UART0_S1_NF_MASK | 
									UART0_S1_FE_MASK | UART0_S1_PF_MASK;
	
	// Send LSB first, do not invert received data
	UART0->S2 = UART0_S2_MSBF(0) | UART0_S2_RXINV(0); 
	
#if USE_UART_INTERRUPTS
	// Enable interrupts. Listing 8.11 on p. 234
	Q_Init(&TxQ);
	Q_Init(&RxQ);

	NVIC_SetPriority(UART0_IRQn, 2); // 0, 1, 2, or 3
	NVIC_ClearPendingIRQ(UART0_IRQn); 
	NVIC_EnableIRQ(UART0_IRQn);

	// Enable receive interrupts but not transmit interrupts yet
	UART0->C2 |= UART_C2_RIE(1);
#endif

	// Enable UART receiver and transmitter
	UART0->C2 |= UART0_C2_RE(1) | UART0_C2_TE(1);	
	
	// Clear the UART RDRF flag
	temp = UART0->D;
	UART0->S1 &= ~UART0_S1_RDRF_MASK;

}
#endif

/* END - UART0 Device Driver 
	Code created by Shannon Strutz
	Date : 5/7/2014
	Licensed under CC BY-NC-SA 3.0
	http://creativecommons.org/licenses/by-nc-sa/3.0/

	Modified by Alex Dean 9/13/2016
*/

/*----------------------------------------------------------------------------
 *         Function 'UART0_Transmit_Poll': Code listing 8.9, p. 233 
 *---------------------------------------------------------------------------*/

void UART0_Transmit_Poll(uint8_t data) {
		while (!(UART0->S1 & UART0_S1_TDRE_MASK))
			;
		UART0->D = data;
}	


/*----------------------------------------------------------------------
 *  Function 'UART_data_packet': Prepare packets with 
								 the recieved sensor information 
								 to be send on UART bus. Data format: 
								[Start bit,Xdata,Ydata,Zdata,Checksum]
 *----------------------------------------------------------------------*/
void UART_data_packet(void){ // 11/29
	  xyz_data[0]=0x88;
		xyz_data[1]=acc_X & 0X00ff; //LSB
		xyz_data[2]=acc_X>>8;       //MSB
		xyz_data[3]=acc_Y & 0X00ff; //LSB
		xyz_data[4]=acc_Y>>8;       //MSB
		xyz_data[5]=acc_Z & 0X00ff; //LSB
		xyz_data[6]=acc_Z>>8;       //MSB
	  xyz_data[7]=xyz_data[0]^xyz_data[1]^xyz_data[2]^xyz_data[3]^xyz_data[4]^xyz_data[5]^xyz_data[6];
}

/*----------------------------------------------------------------------------
 *  Function 'UART0_Transmit_Poll_xyz': Transmit function for UART 
 *---------------------------------------------------------------------------*/
void UART0_Transmit_Poll_xyz(uint8_t *data) {
	int j;
	for ( j=0; j<8; j++ ) {	// 11/30 limit 5 to 8 
	while (!(UART0->S1 & UART0_S1_TDRE_MASK))
			;
		UART0->D = data[j];
   }
}	

/*----------------------------------------------------------------------------
 *  Function 'UART0_Receive_Poll': Recieve function for UART 
 *---------------------------------------------------------------------------*/
uint8_t UART0_Receive_Poll(void) {
		while (!(UART0->S1 & UART0_S1_RDRF_MASK))
			;
		return UART0->D;
}
	

// *******************************END*************************************   
