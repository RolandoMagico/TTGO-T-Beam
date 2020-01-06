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

#ifndef COMPONENTS_NEO6_NEO6_H_
#define COMPONENTS_NEO6_NEO6_H_

/***************************************************************************************************
 * INCLUDES
 **************************************************************************************************/
#include <esp_types.h>

#ifdef __cplusplus
extern "C" {
#endif
/***************************************************************************************************
 * DEFINES
 **************************************************************************************************/

/***************************************************************************************************
 * TYPES
 **************************************************************************************************/
typedef enum
{
  Neo6_False = 0,
  Neo6_True = 1,
} Neo6_BoolType;

typedef enum
{
  Neo6_Off = 0,
  Neo6_On = 1,
} Neo6_StateType;

typedef enum
{
  Neo6_Success,
  Neo6_Failed,
} Neo6_StatusType;

typedef enum
{
  Neo6_OutputFormatUbx = 0,
  Neo6_OutputFormatNmea = 1,
} Neo6_OutputFormat;

typedef enum
{
  Neo6_HardwareReset = 0,
  Neo6_ControlledSoftwareReset = 1,
  Neo6_ControlledSoftwareResetGpsOnly = 2,
  Neo6_HardwareResetAfterShutdown = 4,
  Neo6_ControlledGpsStop = 8,
  Neo6_ControlledGpsStart = 9,
} Neo6_ResetType;

typedef struct
{
  uint32_t TimeOfWeek;
  int32_t Latitude;
  int32_t Longitude;
  int32_t HeightAboveEllipsoid;
  int32_t HeightAboveMeanSeaLevel;
  uint32_t HorizontalAccuracyEstimate;
  uint32_t VertictalAccuracyEstimate;
} Neo6_GeodeticPositionSolutionType;
/***************************************************************************************************
 * DECLARATIONS
 **************************************************************************************************/
extern void Neo6_InitMemory();
extern void Neo6_Init();
extern Neo6_BoolType Neo6_DataAvailable(size_t* dataLength);
extern int Neo6_GetReceivedData(uint8_t* buffer);
extern Neo6_StatusType Neo6_GetGeodeticPositionSolution(Neo6_GeodeticPositionSolutionType* geodeticPositionSolution);
extern void Neo6_GetGpsFixData();
extern Neo6_StatusType Neo6_SetOutputFormat(Neo6_OutputFormat format, Neo6_StateType state);
extern void Neo6_ApplyReset(Neo6_ResetType resetType);
#ifdef __cplusplus
}
#endif

#endif /* COMPONENTS_NEO6_NEO6_H_ */
