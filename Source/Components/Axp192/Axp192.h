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

#ifndef AXP192_AXP192_H_
#define AXP192_AXP192_H_

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
  PowerStatusRegister,
  PowerMode_ChargeStatusRegiser,
  OtgVbusStatusRegister = 0x04,
  DataBufferRegister0 = 0x06,
  DataBufferRegister1,
  DataBufferRegister2,
  DataBufferRegister3,
  DataBufferRegister4,
  DataBufferRegister5,
  ExtenAndDcdc2SwitchControlRegister = 0x10,
  Dcdc1_3AndLDO2_3SwitchControlRegister = 0x12,
  Dcdc2VoltageSettingRegister = 0x23,
  Dcdc1VoltageSettingRegister = 0x26,
  LDO2_3_OutputVoltageSettingRegister = 0x28,
  ChargeControlRegister1 = 0x33,
  BatteryVoltageHigh8Bit = 0x78,
  BatteryVoltageLow4Bit,
  BatteryChargeCurrentHigh8Bit = 0x7A,
  BatteryChargeCurrentLow5Bit,
  BatteryDischargeCurrentHigh8Bit = 0x7C,
  BatteryDischargeCurrentLow5Bit,
  AdcSampleRateRegisterAndTsPinControlRegister = 0x84,
  BatteryChargingCoulombMeterDataRegister31to24 = 0xB0,
  BatteryChargingCoulombMeterDataRegister21to16 = 0xB1,
  BatteryChargingCoulombMeterDataRegister15to08 = 0xB2,
  BatteryChargingCoulombMeterDataRegister07to00 = 0xB3,
  BatteryDischargeCoulombMeterDataRegister31to24 = 0xB4,
  BatteryDischargeCoulombMeterDataRegister21to16 = 0xB5,
  BatteryDischargeCoulombMeterDataRegister15to08 = 0xB6,
  BatteryDischargeCoulombMeterDataRegister07to00 = 0xB7,
} Axp192_RegisterType;

typedef enum
{
  Axp192_Off = 0,
  Axp192_On = 1,
} Axp192_StateType;

typedef enum
{
  Axp192_AdcSampelRate25Hz = 25,
  Axp192_AdcSampelRate50Hz = 50,
  Axp192_AdcSampelRate100Hz = 100,
  Axp192_AdcSampelRate200Hz = 200,
} Axp192_AdcSamplingRateType;

typedef enum
{
  Axp192_ChargeTargetVoltage4100mV = 0,
  Axp192_ChargeTargetVoltage4150mV = 1,
  Axp192_ChargeTargetVoltage4200mV = 2,
  Axp192_ChargeTargetVoltage4360mV = 3,
} Axp192_ChargeTargetVoltageType;
/***************************************************************************************************
 * DECLARATIONS
 **************************************************************************************************/
extern void Axp192_InitMemory();
extern void Axp192_Init();
extern void Axp192_SetDcDc1State(Axp192_StateType state);
extern void Axp192_SetDcDc1Voltage(uint16_t voltage);
extern void Axp192_SetDcDc2State(Axp192_StateType state);
extern void Axp192_SetDcDc2Voltage(uint16_t voltage);
extern void Axp192_SetLdo2State(Axp192_StateType state);
extern void Axp192_SetLdo2Voltage(uint16_t voltage);
extern void Axp192_SetLdo3State(Axp192_StateType state);
extern uint16_t Axp192_GetLdo3Voltage();
extern void Axp192_SetLdo3Voltage(uint16_t voltage);
extern void Axp192_SetExtenState(Axp192_StateType state);
extern uint16_t Axp192_GetBatteryVoltage();
extern uint16_t Axp192_GetBatteryChargeCurrent();
extern uint16_t Axp192_GetBatteryDischargeCurrent();
extern uint32_t Axp192_GetBatteryCharge();
extern Axp192_AdcSamplingRateType Axp192_GetAdcSamplingRate();
extern Axp192_StateType Axp192_GetChargeFunctionState();
extern Axp192_ChargeTargetVoltageType Axp192_GetChargeTargetVoltage();

#endif /* AXP192_AXP192_H_ */
