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
#include "Axp192.h"
#include "Axp192_Cfg.h"
#include "driver/i2c.h"
/***************************************************************************************************
 * DECLARATIONS
 **************************************************************************************************/

/***************************************************************************************************
 * CONSTANTS
 **************************************************************************************************/
const i2c_config_t Axp192_Configuration =
{
  I2C_MODE_MASTER,
  GPIO_NUM_21,
  GPIO_PULLUP_DISABLE,
  GPIO_NUM_22,
  GPIO_PULLUP_DISABLE,
  {
   {
    400000
   }
  }
};
/***************************************************************************************************
 * IMPLEMENTATION
 **************************************************************************************************/
void Axp192_InitMemory()
{
}

void Axp192_Init()
{
  i2c_param_config(I2C_NUM_0, &Axp192_Configuration);
}
