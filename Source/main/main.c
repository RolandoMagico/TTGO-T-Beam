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
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#include "Axp192.h"
#include "Neo6.h"
/***************************************************************************************************
 * DECLARATIONS
 **************************************************************************************************/
static void InitializeMemory();
static void InitializeComponents();
static void Task100ms();
static void Task1000ms();
static UBaseType_t TaskStackMonitoring(UBaseType_t lastRemainingStack);
/***************************************************************************************************
 * IMPLEMENTATION
 **************************************************************************************************/
void app_main()
{
  /* Print chip information */
  esp_chip_info_t chip_info;
  esp_chip_info(&chip_info);
  printf("This is ESP32 chip with %d CPU cores, WiFi%s%s, ", chip_info.cores,
         (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
         (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

  printf("silicon revision %d, ", chip_info.revision);

  printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
         (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

  fflush(stdout);

  InitializeMemory();

  InitializeComponents();

  /* Enable voltage on LDO2 for SX1276 LORA module */
  Axp192_SetLdo2State(Axp192_On);

  /* Enable voltage on LDO3 for NEO6 GPS module */
  Axp192_SetLdo3Voltage(3300);
  Axp192_SetLdo3State(Axp192_On);

  if ((xTaskCreatePinnedToCore(Task100ms, "Task100ms", 4096, NULL, 10, NULL, 0) == pdPASS)
      && (xTaskCreatePinnedToCore(Task1000ms, "Task1000ms", 4096, NULL, 10, NULL, 0) == pdPASS))
  {
    /* The task was created.  Use the task's handle to delete the task. */
    vTaskDelete(NULL);
  }
  else
  {
    for (int i = 10; i >= 0; i--)
    {
      printf("Restarting in %d seconds...\n", i);
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
  }
}

static void InitializeMemory()
{
  Axp192_InitMemory();
  Neo6_InitMemory();
}

static void InitializeComponents()
{
  Axp192_Init();
  Neo6_Init();
}

static void Task100ms(void *pvParameters)
{
  TickType_t xLastWakeTime = xTaskGetTickCount();
  UBaseType_t remainingTaskStack = INT32_MAX;

  size_t gpsDataLength = 0;
  uint8_t receiveBuffer[1024];

  for (;;)
  {
    // Wait for the next cycle.
    vTaskDelayUntil(&xLastWakeTime, 100 / portTICK_PERIOD_MS);
    remainingTaskStack = TaskStackMonitoring(remainingTaskStack);

    Neo6_MainFunction();

    if (Neo6_DataAvailable(&gpsDataLength) == Neo6_True)
    {
      /* Add 1 to gpsDataLength to be able to add \0 at the end of the buffer */
      if ((gpsDataLength + 1) < sizeof(receiveBuffer))
      {
        Neo6_GetReceivedData(receiveBuffer);
        receiveBuffer[gpsDataLength] = '\0';
        printf("Received %d bytes:\n%s\n", gpsDataLength, receiveBuffer);
      }
    }
  }
}

static void Task1000ms()
{
  TickType_t xLastWakeTime = xTaskGetTickCount();
  UBaseType_t remainingTaskStack = INT32_MAX;
  uint16_t lastBatteryVoltage = UINT16_MAX;
  for (;;)
  {
    // Wait for the next cycle.
    vTaskDelayUntil(&xLastWakeTime, 1000 / portTICK_PERIOD_MS);
    remainingTaskStack = TaskStackMonitoring(remainingTaskStack);

    uint16_t currentBatteryVoltage = Axp192_GetBatteryVoltage();
    if (currentBatteryVoltage != lastBatteryVoltage)
    {
      printf("Battery Voltage: %d mV\n", currentBatteryVoltage);
    }

    lastBatteryVoltage = currentBatteryVoltage;
  }
}

static UBaseType_t TaskStackMonitoring(UBaseType_t lastRemainingStack)
{
  UBaseType_t currentRemainingStack = uxTaskGetStackHighWaterMark(NULL);
  if (currentRemainingStack != lastRemainingStack)
  {
    printf("Free stack in %s: %d Bytes\n", pcTaskGetTaskName(NULL), currentRemainingStack);
  }

  return currentRemainingStack;
}
