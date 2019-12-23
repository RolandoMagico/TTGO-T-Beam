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
  Axp192_PowerStatusRegister,
  Axp192_PowerMode_ChargeStatusRegister,
  Axp192_OtgVbusStatusRegister = 0x04,
  Axp192_DataBufferRegister0 = 0x06,
  Axp192_DataBufferRegister1,
  Axp192_DataBufferRegister2,
  Axp192_DataBufferRegister3,
  Axp192_DataBufferRegister4,
  Axp192_DataBufferRegister5,
  Axp192_ExtenAndDcdc2SwitchControlRegister = 0x10,
  Axp192_Dcdc1_3AndLDO2_3SwitchControlRegister = 0x12,
  Axp192_Dcdc2VoltageSettingRegister = 0x23,
  Axp192_Dcdc1VoltageSettingRegister = 0x26,
  Axp192_LDO2_3_OutputVoltageSettingRegister = 0x28,
  Axp192_VoffShutdownVoltageSettingRegister = 0x31,
  Axp192_ShutdownBatteryDetectionChargeLedControlRegister = 0x32,
  Axp192_ChargeControlRegister1 = 0x33,
  Axp192_IrqEnableControlRegister1 = 0x40,
  Axp192_IrqEnableControlRegister2 = 0x41,
  Axp192_IrqEnableControlRegister3 = 0x42,
  Axp192_IrqEnableControlRegister4 = 0x43,
  Axp192_IrqEnableControlRegister5 = 0x4A,
  Axp192_IrqStatusRegister1 = 0x44,
  Axp192_IrqStatusRegister2 = 0x45,
  Axp192_IrqStatusRegister3 = 0x46,
  Axp192_IrqStatusRegister4 = 0x47,
  Axp192_IrqStatusRegister5 = 0x4D,
  Axp192_BatteryVoltageHigh8Bit = 0x78,
  Axp192_BatteryVoltageLow4Bit,
  Axp192_BatteryChargeCurrentHigh8Bit = 0x7A,
  Axp192_BatteryChargeCurrentLow5Bit,
  Axp192_BatteryDischargeCurrentHigh8Bit = 0x7C,
  Axp192_BatteryDischargeCurrentLow5Bit,
  Axp192_AdcEnableSettingRegister1 = 0x82,
  Axp192_AdcEnableSettingRegister2 = 0x83,
  Axp192_AdcSampleRateRegisterAndTsPinControlRegister = 0x84,
  Axp192_TimerControlRegister = 0x8A,
  Axp192_BatteryChargingCoulombMeterDataRegister31to24 = 0xB0,
  Axp192_BatteryChargingCoulombMeterDataRegister21to16 = 0xB1,
  Axp192_BatteryChargingCoulombMeterDataRegister15to08 = 0xB2,
  Axp192_BatteryChargingCoulombMeterDataRegister07to00 = 0xB3,
  Axp192_BatteryDischargeCoulombMeterDataRegister31to24 = 0xB4,
  Axp192_BatteryDischargeCoulombMeterDataRegister21to16 = 0xB5,
  Axp192_BatteryDischargeCoulombMeterDataRegister15to08 = 0xB6,
  Axp192_BatteryDischargeCoulombMeterDataRegister07to00 = 0xB7,
  Axp192_CoulombControlRegister = 0xB8,
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

typedef enum
{
  Axp192_PowerModeA = 0,
  Axp192_PowerModeB = 1,
} Axp192_PowerModeType;

typedef enum
{
  Axp192_BatteryVoltageAdc,
  Axp192_BatteryCurrentAdc,
  Axp192_AcInVoltageAdc,
  Axp192_AcInCurrentAdc,
  Axp192_VbusVoltageAdc,
  Axp192_VbusCurrentAdc,
  Axp192_ApsVoltageAdc,
  Axp192_TsPinAdc,
  Axp192_InternalTemperatureMonitoringAdc,
  Axp192_Gpio0Adc,
  Axp192_Gpio1Adc,
  Axp192_Gpio2Adc,
  Axp192_Gpio3Adc,
} Axp192_AdcType;

/*
 * Enumeration of all interrupt sources.
 * The enumeration values are chosen so that the value modulo 8 results in the bit number
 * of the corresponding register.
 */
typedef enum
{
  /* IRQ Enable Control Register 1 */
  /* Bit 0 is not used */
  /* Bit 1 */ Axp192_VbusAvailableButLessThanVholdIrq = 1,
  /* Bit 2 */ Axp192_VbusRemovedIrq = 2,
  /* Bit 3 */ Axp192_VbusAccessIrq = 3,
  /* Bit 4 */ Axp192_VbusOverVoltageIrq = 4,
  /* Bit 5 */ Axp192_AcInRemovedIrq = 5,
  /* Bit 6 */ Axp192_AcInAccessIrq = 6,
  /* Bit 7 */ Axp192_AcInOverVoltageIrq = 7,

  /* IRQ Enable Control Register 2 */
  /* Bit 0 */ Axp192_BatteryLowTemperatureIrq = 8,
  /* Bit 1 */ Axp192_BatteryOverTemperatureIrq = 9,
  /* Bit 2 */ Axp192_FinishedChargingIrq = 10,
  /* Bit 3 */ Axp192_ChargingIrq = 11,
  /* Bit 4 */ Axp192_ExitBatteryActivationModeIrq = 12,
  /* Bit 5 */ Axp192_BatteryActivationModeIrq = 13,
  /* Bit 6 */ Axp192_BatteryRemovedIrq = 14,
  /* Bit 7 */ Axp192_BatteryConnectedIrq = 15,

  /* IRQ Enable Control Register 3 */
  /* Bit 0 */ Axp192_LongButtonIrq = 16,
  /* Bit 1 */ Axp192_ShortButtonIrq = 17,
  /* Bit 3 */ Axp192_DcDc3OutputVoltageLessThanSetVoltageIrq = 19,
  /* Bit 4 */ Axp192_DcDc2OutputVoltageLessThanSetVoltageIrq = 20,
  /* Bit 5 */ Axp192_DcDc1OutputVoltageLessThanSetVoltageIrq = 21,
  /* Bit 6 */ Axp192_ChargingCurrentLessThanSetCurrentIrq = 22,
  /* Bit 7 */ Axp192_InternalOverTemperatureIrq = 23,

  /* IRQ Enable Control Register 4 */
  /* Bit 0 */ Axp192_LowPressureIrq = 24,
  /* Bit 1 is not used */
  /* Bit 2 */ Axp192_VbusSessionEndIrq = 26,
  /* Bit 3 */ Axp192_VbusSessionABIrq = 27,
  /* Bit 4 */ Axp192_VbusInvalidIrq = 28,
  /* Bit 5 */ Axp192_VbusValidIrq = 29,
  /* Bit 6 */ Axp192_N_OE_ShutdownIrq = 30,
  /* Bit 7 */ Axp192_N_OE_BootIrq = 31,

  /* IRQ Enable Control Register 5 */
  /* Bit 0 */ Axp192_Gpio0InputEdgeTriggerIrq = 32,
  /* Bit 1 */ Axp192_Gpio1InputEdgeTriggerIrq = 33,
  /* Bit 2 */ Axp192_Gpio2InputEdgeTriggerIrq = 34,
  /* Bits 3-6 are not used */
  /* Bit 7 */ Axp192_TimerExpiredIrq = 39,
} Axp192_IrqType;
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
extern Axp192_PowerModeType Axp192_GetPowerMode();
extern Axp192_StateType Axp192_GetPwronWakeupFunctionState();
extern void Axp192_SetPwronWakeupFunctionState(Axp192_StateType state);
extern void Axp192_Shutdown();
extern void Axp192_SetTimer(uint8_t minutes);
extern void Axp192_SetAdcState(Axp192_AdcType adc, Axp192_StateType state);
extern void Axp192_SetCoulombSwitchControlState(Axp192_StateType state);
extern Axp192_StateType Axp192_GetIrqState(Axp192_IrqType irq);
extern void Axp192_SetIrqState(Axp192_IrqType irq, Axp192_StateType state);
#endif /* AXP192_AXP192_H_ */
