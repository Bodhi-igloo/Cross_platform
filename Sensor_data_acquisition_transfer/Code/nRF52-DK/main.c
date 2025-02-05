/** @} */

/**
 * Copyright (c) 2014 - 2020, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/** @file
 * @defgroup uart_example_main main.c
 * @{
 * @ingroup uart_example
 * @brief UART Example Application main file.
 *
 * This file contains the source code for a sample application using UART.
 *
 */

#include "app_error.h"
#include "app_uart.h"
#include "bsp.h"
#include "nrf.h"
#include "nrf_delay.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#if defined(UART_PRESENT)
#include "nrf_uart.h"
#endif
#if defined(UARTE_PRESENT)
#include "nrf_uarte.h"
#endif

//#define ENABLE_LOOPBACK_TEST  /**< if defined, then this example will be a loopback test, which means that TX should be connected to RX to get data loopback. */

#define MAX_TEST_DATA_BYTES (15U) /**< max number of test bytes to be used for tx and rx. */
#define UART_TX_BUF_SIZE 256      /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE 256      /**< UART RX buffer size. */

void uart_error_handle(app_uart_evt_t *p_event) {
  if (p_event->evt_type == APP_UART_COMMUNICATION_ERROR) {
    APP_ERROR_HANDLER(p_event->data.error_communication);
  } else if (p_event->evt_type == APP_UART_FIFO_ERROR) {
    APP_ERROR_HANDLER(p_event->data.error_code);
  }
}

#ifdef ENABLE_LOOPBACK_TEST
/* Use flow control in loopback test. */
#define UART_HWFC APP_UART_FLOW_CONTROL_ENABLED

/** @brief Function for setting the @ref ERROR_PIN high, and then enter an infinite loop.
 */
static void show_error(void) {

  bsp_board_leds_on();
  while (true) {
    // Do nothing.
  }
}

/** @brief Function for testing UART loop back.
 *  @details Transmitts one character at a time to check if the data received from the loopback is same as the transmitted data.
 *  @note  @ref TX_PIN_NUMBER must be connected to @ref RX_PIN_NUMBER)
 */
static void uart_loopback_test() {
  uint8_t *tx_data = (uint8_t *)("\r\nLOOPBACK_TEST\r\n");
  uint8_t rx_data;

  // Start sending one byte and see if you get the same
  for (uint32_t i = 0; i < MAX_TEST_DATA_BYTES; i++) {
    uint32_t err_code;
    while (app_uart_put(tx_data[i]) != NRF_SUCCESS)
      ;

    nrf_delay_ms(10);
    err_code = app_uart_get(&rx_data);

    if ((rx_data != tx_data[i]) || (err_code != NRF_SUCCESS)) {
      show_error();
    }
  }
  return;
}
#else

/* When UART is used for communication with the host do not use flow control.*/
#define UART_HWFC APP_UART_FLOW_CONTROL_DISABLED
#endif

/**
 * @brief Function for main application entry.
 */
int main(void) {
  //declare variables
  uint32_t err_code;
  uint8_t Rx_data;
  uint8_t array_len;
  uint8_t data_array[20];
  int8_t unsigned_data_array[20];
  int16_t x, y, z;
  bsp_board_init(BSP_INIT_LEDS); // initlize the LED's configurations 
  // UART configuration
  const app_uart_comm_params_t comm_params =
  {
    RX_PIN_NUMBER,
    TX_PIN_NUMBER,
    RTS_PIN_NUMBER,
    CTS_PIN_NUMBER,
    UART_HWFC,
    false,
#if defined(UART_PRESENT)
    NRF_UART_BAUDRATE_115200
#else
    NRF_UARTE_BAUDRATE_115200
#endif
  };

  APP_UART_FIFO_INIT(&comm_params,
      UART_RX_BUF_SIZE,
      UART_TX_BUF_SIZE,
      uart_error_handle,
      APP_IRQ_PRIORITY_LOWEST,
      err_code);

  APP_ERROR_CHECK(err_code);

#ifndef ENABLE_LOOPBACK_TEST
  printf("\n\r UART started.");
  printf("\n");
  while (true) {
    //nrf_delay_ms(100);
    while (app_uart_get(&Rx_data) != NRF_ERROR_NOT_FOUND) { // read the UART data on RX unlees there is error
      data_array[array_len] = Rx_data;                       // write the received RX byte to an data array 
      unsigned_data_array[array_len]= data_array[array_len]; // copy the array to an unsigned data array for logging -ve values
      array_len += 1;                                        // increment the array_len for every RX read byte
      //printf("len= %d, data=%d\n", array_len, Rx_data);
      //printf("\n");
      if (data_array[0] != 0x88) {                           // check the start code of the array, if not equal to 0x88 set array and length to "0"
        array_len = 0;                         
        data_array[array_len] = 0;
      }
      
      if (array_len == 8 ) {                                 // if array len =8 ; this is the packet we are looking for with sensor data
        uint8_t checksum = data_array[0] ^ data_array[1] ^ data_array[2] ^ data_array[3]^data_array[4] ^ data_array[5] ^ data_array[6]; // compute the checksum of the first 7 bytes(startcode + data array)
        if(data_array[7] == checksum){                        // compare if the calculated checksum equal is equal to last byte of the data array for error check
        x = (data_array[2] << 8) | data_array[1] ;            // reframe the X,Y,Z sensor data from the received data array  
        y = (data_array[4] << 8) | data_array[3] ;
        z = (data_array[6] << 8) | data_array[5] ;
        printf("\n\r X = %d Y = %d Z = %d", x,y,z);           // print the sensor data
        array_len = 0;                                        // set array and length to "0" to receive new packets        
        data_array[array_len] = 0;
        bsp_board_leds_on();                                  // Turn ON LED's on the board
        nrf_delay_ms(50);
        bsp_board_leds_off();                                 // Turn OFF LED's on the board
        }
      }
    }
  }
#else

  // This part of the example is just for testing the loopback .
  while (true) {
    uart_loopback_test();
  }
#endif
}
