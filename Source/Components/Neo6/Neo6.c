/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the LICENSE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */


/***************************************************************************************************
 * INCLUDES
 **************************************************************************************************/
#include "Neo6.h"
#include "Neo6_Cfg.h"
#include "driver/uart.h"
/***************************************************************************************************
 * DECLARATIONS
 **************************************************************************************************/

/***************************************************************************************************
 * CONSTANTS
 **************************************************************************************************/

/***************************************************************************************************
 * IMPLEMENTATION
 **************************************************************************************************/
void Neo6_InitMemory()
{
}

void Neo6_Init()
{
  QueueHandle_t uart_queue;
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
  ESP_ERROR_CHECK(uart_driver_install(NEO6_UART_PERIPHERAL, NEO6_UART_BUFFER_SIZE, NEO6_UART_BUFFER_SIZE, 10, &uart_queue, 0));

  ESP_ERROR_CHECK(uart_flush(NEO6_UART_PERIPHERAL));
}

Neo6_BoolType Neo6_DataAvailable(size_t* dataLength)
{
  ESP_ERROR_CHECK(uart_get_buffered_data_len(NEO6_UART_PERIPHERAL, dataLength));
  return (*dataLength) > 0;
}
