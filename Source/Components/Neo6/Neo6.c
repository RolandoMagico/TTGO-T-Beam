/***************************************************************************************************
 * Copyright 2019 ContextQuickie
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 **************************************************************************************************/

/***************************************************************************************************
 * INCLUDES
 **************************************************************************************************/
#include "Neo6.h"
#include "Neo6_Cfg.h"
#include "driver/uart.h"

/***************************************************************************************************
 * DEFINES
 **************************************************************************************************/

/***************************************************************************************************
 * TYPES
 **************************************************************************************************/

/***************************************************************************************************
 * DECLARATIONS
 **************************************************************************************************/
static void Neo6_MemCopy(uint8_t* source, uint8_t* destination, size_t count);
/***************************************************************************************************
 * CONSTANTS
 **************************************************************************************************/

/***************************************************************************************************
 * VARIABLES
 **************************************************************************************************/
static uint8_t Neo6_RecveiveBuffer[NEO6_UART_BUFFER_SIZE];
static uint32_t Neo6_ReceiveBufferPosition;
static uint32_t Neo6_ReceiveTimeoutCounter;

static uint8_t Neo6_ReadBuffer[NEO6_UART_BUFFER_SIZE];
static uint32_t Neo6_ReadBufferLength;
/***************************************************************************************************
 * IMPLEMENTATION
 **************************************************************************************************/
void Neo6_InitMemory()
{
  Neo6_ReceiveBufferPosition = 0;
  Neo6_ReadBufferLength = 0;
  Neo6_ReceiveTimeoutCounter = 0;
}

void Neo6_Init()
{
  uart_config_t uart_config = {
      .baud_rate = 9600,
      .data_bits = UART_DATA_8_BITS,
      .parity = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
  };
  // Configure UART parameters
  ESP_ERROR_CHECK(uart_param_config(NEO6_UART_PERIPHERAL, &uart_config));

  // Set UART pins
  ESP_ERROR_CHECK(uart_set_pin(NEO6_UART_PERIPHERAL, NEO6_UART_TX_PIN, NEO6_UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

  // Install UART driver using an event queue here
  ESP_ERROR_CHECK(uart_driver_install(NEO6_UART_PERIPHERAL, NEO6_UART_BUFFER_SIZE, 0, 0, NULL, 0));

  ESP_ERROR_CHECK(uart_flush(NEO6_UART_PERIPHERAL));
}

Neo6_BoolType Neo6_DataAvailable(size_t* dataLength)
{
  *dataLength = Neo6_ReadBufferLength;
  return (*dataLength) > 0;
}

int Neo6_GetReceivedData(uint8_t* buffer)
{
  uint32_t reultBufferLength = Neo6_ReadBufferLength;
  Neo6_MemCopy(Neo6_ReadBuffer, buffer, reultBufferLength);

  /* Setting Neo6_ReadBufferLength to 0 means the data has been read */
  Neo6_ReadBufferLength = 0;
  return reultBufferLength;
}

void Neo6_MainFunction()
{
  size_t dataLength;
  ESP_ERROR_CHECK(uart_get_buffered_data_len(NEO6_UART_PERIPHERAL, &dataLength));
  if (dataLength == 0)
  {
    Neo6_ReceiveTimeoutCounter++;
    if (Neo6_ReceiveTimeoutCounter > 2)
    {
      /* No data received since the last call */
      if ((Neo6_ReceiveBufferPosition != 0) &&                      /* Check if there are received data */
          (Neo6_ReadBuffer[0] = '$') &&                             /* Check for a valid start delimiter */
          (Neo6_ReadBuffer[Neo6_ReceiveBufferPosition - 1] = '\n')) /* Check for a valid end delimiter */
      {
        /* Assume that data reception is complete and copy date to the read buffer */
        Neo6_MemCopy(Neo6_RecveiveBuffer, Neo6_ReadBuffer, Neo6_ReceiveBufferPosition);
        Neo6_ReadBufferLength = Neo6_ReceiveBufferPosition;
      }

      Neo6_ReceiveBufferPosition = 0;
      Neo6_ReceiveTimeoutCounter = 0;
    }
  }
  else if ((Neo6_ReceiveBufferPosition + dataLength) < NEO6_UART_BUFFER_SIZE)
  {
    uart_read_bytes(NEO6_UART_PERIPHERAL, &(Neo6_RecveiveBuffer[Neo6_ReceiveBufferPosition]), dataLength, 1);
    Neo6_ReceiveBufferPosition += dataLength;
    Neo6_ReceiveTimeoutCounter = 0;
  }
  else
  {
    printf("BUFFER TOO SMALL!!!!!\n");
  }
}

static void Neo6_MemCopy(uint8_t* source, uint8_t* destination, size_t count)
{
  for (uint32_t position = 0; position < count; position++)
  {
    destination[position] = source[position];
  }
}
