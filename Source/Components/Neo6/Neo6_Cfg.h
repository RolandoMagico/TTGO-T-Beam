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

#ifndef COMPONENTS_NEO6_NEO6_CFG_H_
#define COMPONENTS_NEO6_NEO6_CFG_H_

/***************************************************************************************************
 * INCLUDES
 **************************************************************************************************/
#include "driver/gpio.h"
#include "driver/uart.h"

/***************************************************************************************************
 * DEFINES
 **************************************************************************************************/
#define NEO6_UART_PERIPHERAL                          UART_NUM_1
#define NEO6_UART_RX_PIN                              GPIO_NUM_34
#define NEO6_UART_TX_PIN                              GPIO_NUM_12
#define NEO6_UART_BUFFER_SIZE                         (2048u)
#define NEO6_UART_READ_TIMEOUT                        (100)
/***************************************************************************************************
 * TYPES
 **************************************************************************************************/

/***************************************************************************************************
 * DECLARATIONS
 **************************************************************************************************/

#endif /* COMPONENTS_NEO6_NEO6_CFG_H_ */
