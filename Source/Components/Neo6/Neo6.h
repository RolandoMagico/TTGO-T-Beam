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
/***************************************************************************************************
 * DECLARATIONS
 **************************************************************************************************/
extern void Neo6_InitMemory();
extern void Neo6_Init();
extern Neo6_BoolType Neo6_DataAvailable(size_t* dataLength);
extern int Neo6_GetReceivedData(uint8_t* buffer);
extern void Neo6_MainFunction();
#endif /* COMPONENTS_NEO6_NEO6_H_ */
