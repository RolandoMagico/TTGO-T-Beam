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
#ifndef MAIN_THETHINGSNETWORK_CFG_H_
#define MAIN_THETHINGSNETWORK_CFG_H_

/***************************************************************************************************
 * INCLUDES
 **************************************************************************************************/
#include "driver/gpio.h"
#include "driver/spi_master.h"
/***************************************************************************************************
 * DEFINES
 **************************************************************************************************/
/* SPI pins */
#define TTN_PIN_SPI_MISO                (GPIO_NUM_19)
#define TTN_PIN_SPI_MOSI                (GPIO_NUM_27)
#define TTN_PIN_SPI_SCLK                (GPIO_NUM_5)
#define TTN_PIN_NSS                     (GPIO_NUM_18)

#define TTN_SPI_HOST                    (HSPI_HOST)
#define TTN_SPI_DMA_CHAN                (1)

/* Non SPI pins */
#define TTN_PIN_RXTX                    (TTN_NOT_CONNECTED)
#define TTN_PIN_RST                     (GPIO_NUM_14)
#define TTN_PIN_DIO0                    (GPIO_NUM_26)
#define TTN_PIN_DIO1                    (GPIO_NUM_33)

/* TTN access data */
#define TTN_DEVICE_EUI                  "????????????????"
#define TTN_APPLICATION_EUI             "????????????????"
#define TTN_APPLICATION_SESSION_KEY     "????????????????????????????????"

/***************************************************************************************************
 * TYPES
 **************************************************************************************************/

/***************************************************************************************************
 * DECLARATIONS
 **************************************************************************************************/

#endif /* MAIN_THETHINGSNETWORK_CFG_H_ */
