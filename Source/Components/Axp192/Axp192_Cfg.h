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

#ifndef AXP192_AXP192_CFG_H_
#define AXP192_AXP192_CFG_H_

/***************************************************************************************************
 * INCLUDES
 **************************************************************************************************/
#include "driver/i2c.h"

/***************************************************************************************************
 * DEFINES
 **************************************************************************************************/
/**
 * Set the slave address of the AXP192 device.
 */
#define AXP_192_SLAVE_ADDRESS                        (0x34)

/**
 * Set the I2C port which is used.
 */
#define AXP_192_I2C_PORT                             (I2C_NUM_0)

/**
 * Enable/disable debug output. Define either
 * AXP_192_DEBUG_OUTPUT_ENABLED
 * or
 * AXP_192_DEBUG_OUTPUT_DISABLED
 */
#define AXP_192_DEBUG_OUTPUT_ENABLED
/***************************************************************************************************
 * DECLARATIONS
 **************************************************************************************************/
extern const i2c_config_t Axp192_Configuration;


#endif /* AXP192_AXP192_CFG_H_ */
