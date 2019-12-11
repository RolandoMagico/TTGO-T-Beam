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
/***************************************************************************************************
 * IMPLEMENTATION
 **************************************************************************************************/
void app_main()
{
    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is ESP32 chip with %d CPU cores, WiFi%s%s, ",
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    fflush(stdout);

    InitializeMemory();

    InitializeComponents();

    /* TODO: Just for testing */
    /* Axp192_SetDcDc1Voltage(3300); */
    /* Axp192_SetDcDc1State(Axp192_On); */

    /* TODO: Enable voltage on LDO2 for SX1276 LORA module */

    /* TODO: Unclear if it is needed */
    Axp192_SetLdo2State(Axp192_On);
    Axp192_SetDcDc1State(Axp192_On);
    Axp192_SetDcDc2State(Axp192_On);
    Axp192_SetExtenState(Axp192_On);
    Axp192_SetDcDc1Voltage(3300);
    Axp192_SetDcDc2Voltage(2275);

    /* Enable voltage on LDO3 for NEO6 GPS module */
    Axp192_SetLdo3Voltage(3300);
    Axp192_SetLdo3State(Axp192_On);

    while (1)
    {
      size_t gpsDataLength = 0;
      /* Intended endless loop */
      if (Neo6_DataAvailable(&gpsDataLength) == Neo6_True)
      {
        printf("Received %d bytes\n", gpsDataLength);
      }
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    for (int i = 10; i >= 0; i--) {
        printf("Restarting in %d seconds...\n", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
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
