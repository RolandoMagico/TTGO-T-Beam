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
#ifndef COMPONENTS_DISPLAY_DISPLAY_CFG_H_
#define COMPONENTS_DISPLAY_DISPLAY_CFG_H_

/***************************************************************************************************
 * INCLUDES
 **************************************************************************************************/
#include "driver/i2c.h"

/***************************************************************************************************
 * DEFINES
 **************************************************************************************************/
/**
 * Set the slave address of the display device.
 */
#define DISPLAY_SLAVE_ADDRESS                        (0x3C)

/**
 * Set the I2C port which is used.
 */
#define DISPLAY_I2C_PORT                             (I2C_NUM_0)
/***************************************************************************************************
 * TYPES
 **************************************************************************************************/

/***************************************************************************************************
 * DECLARATIONS
 **************************************************************************************************/

#endif /* COMPONENTS_DISPLAY_DISPLAY_CFG_H_ */
