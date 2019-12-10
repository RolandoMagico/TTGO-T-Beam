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
 * DEFINES
 **************************************************************************************************/
#ifdef AXP_192_DEBUG_OUTPUT_ENABLED
#define AXP_192_PRINTF(format, ...) \
  printf("File %s, function %s, line %d: " format "\n", __FILE__, __FUNCTION__, __LINE__ __VA_OPT__(,) __VA_ARGS__)
#else
#define AXP_192_PRINTF(format, ...)
#endif
/***************************************************************************************************
 * DECLARATIONS
 **************************************************************************************************/
static void Axp192_ReadI2cData();
static void Axp192_WriteI2cData(uint8_t*, size_t);
static void Axp192_ReadRegister(Axp192_RegisterType, uint8_t*);
static void Axp192_WriteRegister(Axp192_RegisterType, uint8_t);
/***************************************************************************************************
 * CONSTANTS
 **************************************************************************************************/

/***************************************************************************************************
 * IMPLEMENTATION
 **************************************************************************************************/
void Axp192_InitMemory()
{
}

void Axp192_Init()
{
  uint8_t rxBuffer = 0x00u;
  i2c_param_config(AXP_192_I2C_PORT, &Axp192_Configuration);
  i2c_driver_install(AXP_192_I2C_PORT, Axp192_Configuration.mode, 0, 0, ESP_INTR_FLAG_IRAM);
  Axp192_ReadRegister(DataBufferRegister0, &rxBuffer);
  Axp192_ReadRegister(DataBufferRegister1, &rxBuffer);
  Axp192_ReadRegister(DataBufferRegister2, &rxBuffer);
  Axp192_ReadRegister(DataBufferRegister3, &rxBuffer);
  Axp192_ReadRegister(DataBufferRegister4, &rxBuffer);
  Axp192_ReadRegister(DataBufferRegister5, &rxBuffer);
}

void Axp192_SetDcDc1State(Axp192_StateType state)
{

}

static void Axp192_ReadRegister(Axp192_RegisterType registerAddress, uint8_t* buffer)
{
  uint8_t txData = registerAddress;
  Axp192_WriteI2cData(&txData, 1);
  Axp192_ReadI2cData(buffer, 1);
  AXP_192_PRINTF("Reading register 0x%02X done. Read value: 0x%02X\n", (int)registerAddress, (int)(*buffer));
}

static void Axp192_WriteRegister(Axp192_RegisterType registerAddress, uint8_t data)
{
  uint8_t txData[2] = { registerAddress, data };
  Axp192_WriteI2cData(txData, 2);
}

static void Axp192_ReadI2cData(uint8_t* buffer, size_t bufferLength)
{
  i2c_cmd_handle_t i2c_cmd_handle = i2c_cmd_link_create();
  if (i2c_master_start(i2c_cmd_handle) != ESP_OK)
  {
    AXP_192_PRINTF("Axp192: Error during i2c_master_start");
  }
  else if (i2c_master_write_byte(i2c_cmd_handle, (AXP_192_SLAVE_ADDRESS << 1) | I2C_MASTER_READ, true) != ESP_OK)
  {
    AXP_192_PRINTF("Axp192: Error during i2c_master_write_byte");
  }
  else if (i2c_master_read_byte(i2c_cmd_handle, buffer, I2C_MASTER_NACK) != ESP_OK)
  {
    AXP_192_PRINTF("Axp192: Error during i2c_master_read");
  }
  else if (i2c_master_stop(i2c_cmd_handle) != ESP_OK)
  {
    AXP_192_PRINTF("Axp192: Error during i2c_master_stop");
  }
  else if (i2c_master_cmd_begin(AXP_192_I2C_PORT, i2c_cmd_handle, 1000 / portTICK_RATE_MS) != ESP_OK)
  {
    AXP_192_PRINTF("Axp192: Error during i2c_master_cmd_begin");
  }
  else
  {
    i2c_cmd_link_delete(i2c_cmd_handle);
  }
}

static void Axp192_WriteI2cData(uint8_t* data, size_t dataLength)
{
  i2c_cmd_handle_t i2c_cmd_handle = i2c_cmd_link_create();
  if (i2c_master_start(i2c_cmd_handle) != ESP_OK)
  {
    AXP_192_PRINTF("Axp192: Error during i2c_master_start\n");
  }
  else if (i2c_master_write_byte(i2c_cmd_handle, (AXP_192_SLAVE_ADDRESS << 1) | I2C_MASTER_WRITE, true) != ESP_OK)
  {
    AXP_192_PRINTF("Axp192: Error during i2c_master_write_byte\n");
  }
  else if (i2c_master_write(i2c_cmd_handle, data, dataLength, true) != ESP_OK)
  {
    AXP_192_PRINTF("Axp192: Error during i2c_master_write_byte\n");
  }
  else if (i2c_master_stop(i2c_cmd_handle) != ESP_OK)
  {
    AXP_192_PRINTF("Axp192: Error during i2c_master_stop\n");
  }
  else if (i2c_master_cmd_begin(AXP_192_I2C_PORT, i2c_cmd_handle, 5000 / portTICK_RATE_MS) != ESP_OK)
  {
    AXP_192_PRINTF("Axp192: Error during i2c_master_cmd_begin\n");
  }
  else
  {
    i2c_cmd_link_delete(i2c_cmd_handle);
  }
}
