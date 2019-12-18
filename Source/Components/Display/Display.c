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
#include "u8g2.h"
#include "esp_log.h"

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

/***************************************************************************************************
 * IMPLEMENTATION
 **************************************************************************************************/
void Display_InitMemory()
{
}

void Display_Init()
{
  u8g2_t u8g2;
  u8g2_Setup_sh1106_i2c_128x64_noname_1(&u8g2, U8G2_R0, &Display_CommunicationCallback, &Display_GpioAndDelayCallback);
  u8g2_InitDisplay(&u8g2); // send init sequence to the display, display is in sleep mode after this,
  u8g2_SetPowerSave(&u8g2, 0); // wake up display
}

static uint8_t Display_GpioAndDelayCallback(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
  uint8_t returnValue = 0;
  switch (msg)
  {
    case U8X8_MSG_GPIO_AND_DELAY_INIT:
      ESP_LOGI(__FUNCTION__, "Received event U8X8_MSG_GPIO_AND_DELAY_INIT");
      break;
    case U8X8_MSG_GPIO_RESET:
      ESP_LOGI(__FUNCTION__, "Received event U8X8_MSG_GPIO_RESET");
      break;
    case U8X8_MSG_DELAY_MILLI:
      ESP_LOGI(__FUNCTION__, "Received event U8X8_MSG_GPIO_AND_DELAY_INIT");
      vTaskDelay(arg_int / portTICK_PERIOD_MS);
      break;
    default:
      ESP_LOGE(__FUNCTION__, "Unknown event %d", msg);
      returnValue = 1;
      break;
  }

  return returnValue;
}

static uint8_t Display_CommunicationCallback(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int,
                                             void *arg_ptr)
{
  uint8_t returnValue = 0;
  switch (msg)
  {
    case U8X8_MSG_BYTE_SEND:
      ESP_LOGI(__FUNCTION__, "Received event U8X8_MSG_BYTE_SEND");
      break;
    case U8X8_MSG_BYTE_INIT:
      ESP_LOGI(__FUNCTION__, "Received event U8X8_MSG_BYTE_INIT");
      break;
    case U8X8_MSG_BYTE_SET_DC:
      ESP_LOGI(__FUNCTION__, "Received event U8X8_MSG_BYTE_SET_DC");
      break;
    case U8X8_MSG_BYTE_START_TRANSFER:
      ESP_LOGI(__FUNCTION__, "Received event U8X8_MSG_BYTE_START_TRANSFER");
      break;
    case U8X8_MSG_BYTE_END_TRANSFER:
      ESP_LOGI(__FUNCTION__, "Received event U8X8_MSG_BYTE_END_TRANSFER");
      break;
    default:
      ESP_LOGE(__FUNCTION__, "Unknown event %d", msg);
      returnValue = 1;
      break;
  }

  return returnValue;
}
