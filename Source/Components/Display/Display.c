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
#include "Display.h"
#include "Display_Cfg.h"

#include "u8g2.h"

#include "esp_log.h"
#include "driver/i2c.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
/***************************************************************************************************
 * DEFINES
 **************************************************************************************************/

/***************************************************************************************************
 * TYPES
 **************************************************************************************************/

/***************************************************************************************************
 * DECLARATIONS
 **************************************************************************************************/
static uint8_t Display_CommunicationCallback(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int,
                                             void *arg_ptr);
static uint8_t Display_GpioAndDelayCallback(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int,
                                            void *arg_ptr);
/***************************************************************************************************
 * CONSTANTS
 **************************************************************************************************/

/***************************************************************************************************
 * VARIABLES
 **************************************************************************************************/
static uint8_t Display_TxData[32];
static uint8_t Display_TxDataLength;

/***************************************************************************************************
 * IMPLEMENTATION
 **************************************************************************************************/
void Display_InitMemory()
{
  Display_TxDataLength = 0;
}

void Display_Init()
{
  u8g2_t u8g2;
  u8g2_Setup_sh1106_i2c_128x64_noname_1(&u8g2, U8G2_R0, &Display_CommunicationCallback, &Display_GpioAndDelayCallback);
  u8g2_InitDisplay(&u8g2); // send init sequence to the display, display is in sleep mode after this,
  u8g2_SetPowerSave(&u8g2, 0); // wake up display
  u8g2_SetI2CAddress(&u8g2, DISPLAY_I2C_PORT);
  u8g2_ClearDisplay(&u8g2);
}

static uint8_t Display_GpioAndDelayCallback(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
  uint8_t returnValue = 0;
  switch (msg)
  {
    case U8X8_MSG_GPIO_AND_DELAY_INIT:
      /* Nothing to do here */
      break;
    case U8X8_MSG_GPIO_RESET:
      /* Nothing to do here */
      break;
    case U8X8_MSG_DELAY_MILLI:
      vTaskDelay(arg_int / portTICK_PERIOD_MS);
      break;
    default:
      ESP_LOGE(__FUNCTION__, "Unknown event %d", msg);
      returnValue = 1;
      break;
  }

  return returnValue;
}

static uint8_t Display_CommunicationCallback(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
  uint8_t returnValue = 0;
  i2c_cmd_handle_t i2c_cmd_handle;
  switch (msg)
  {
    case U8X8_MSG_BYTE_SEND:
      /* Copy data to internal TX buffer:
       * - arg_ptr contains the data to transmit
       * - arg_int contains the length of the data
       */
      for (uint8_t position = 0; position < arg_int; position++)
      {
        Display_TxData[Display_TxDataLength] = ((uint8_t*)arg_ptr)[position];
        Display_TxDataLength++;
      }
      break;
    case U8X8_MSG_BYTE_INIT:
      /* Nothing to do here */
      break;
    case U8X8_MSG_BYTE_SET_DC:
      /* Nothing to do here */
      break;
    case U8X8_MSG_BYTE_START_TRANSFER:
      Display_TxDataLength = 0;
      break;
    case U8X8_MSG_BYTE_END_TRANSFER:
      i2c_cmd_handle = i2c_cmd_link_create();
      ESP_ERROR_CHECK(i2c_master_start(i2c_cmd_handle));
      ESP_ERROR_CHECK(i2c_master_write_byte(i2c_cmd_handle, (DISPLAY_SLAVE_ADDRESS << 1) | I2C_MASTER_WRITE, true));
      ESP_ERROR_CHECK(i2c_master_write(i2c_cmd_handle, Display_TxData, Display_TxDataLength, true));
      ESP_ERROR_CHECK(i2c_master_stop(i2c_cmd_handle));
      ESP_ERROR_CHECK(i2c_master_cmd_begin(DISPLAY_I2C_PORT, i2c_cmd_handle, 5000 / portTICK_RATE_MS));
      i2c_cmd_link_delete(i2c_cmd_handle);
      break;
    default:
      ESP_LOGE(__FUNCTION__, "Unknown event %d", msg);
      returnValue = 1;
      break;
  }

  return returnValue;
}
