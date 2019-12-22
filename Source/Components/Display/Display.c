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
 * Decsription
 * Implementation for a Adafruit OLED display wit a resolution of 128x64 pxiels.
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
static u8g2_t Dispaly_u8g2_Instance;
/***************************************************************************************************
 * IMPLEMENTATION
 **************************************************************************************************/
void Display_InitMemory()
{
  Display_TxDataLength = 0;
}

void Display_Init()
{
  u8g2_Setup_sh1106_i2c_128x64_noname_f(&Dispaly_u8g2_Instance, U8G2_R0, &Display_CommunicationCallback, &Display_GpioAndDelayCallback);

  /* Send initialization sequence to the display, display is in sleep mode afterwards */
  u8g2_InitDisplay(&Dispaly_u8g2_Instance);

  /* Wake up display */
  u8g2_SetPowerSave(&Dispaly_u8g2_Instance, 0);

  /* TODO: Currently the I2C address is only set but not used anywhere */
  u8g2_SetI2CAddress(&Dispaly_u8g2_Instance, DISPLAY_I2C_PORT);

  Display_Clear();
  u8g2_SetFont(&Dispaly_u8g2_Instance, u8g2_font_ncenB10_tr);
}

void Display_Clear()
{
  u8g2_ClearDisplay(&Dispaly_u8g2_Instance);
}

void Display_DrawString(uint8_t x, uint8_t y, const char *str)
{
  u8g2_DrawStr(&Dispaly_u8g2_Instance, x, y, str);
}

void Display_SendBuffer()
{
  u8g2_SendBuffer(&Dispaly_u8g2_Instance);
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
