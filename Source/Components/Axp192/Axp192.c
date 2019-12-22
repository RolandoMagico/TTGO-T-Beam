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
#include "Axp192.h"
#include "Axp192_Cfg.h"
#include "driver/i2c.h"
#include "esp_log.h"

/***************************************************************************************************
 * DEFINES
 **************************************************************************************************/
/**
 * Minimum and maximum voltages for DC-DC1
 */
#define AXP_192_DCDC1_MIN_VOLTAGE             (700u)
#define AXP_192_DCDC1_MAX_VOLTAGE             (3500u)

/**
 * Minimum and maximum voltages for DC-DC2
 */
#define AXP_192_DCDC2_MIN_VOLTAGE             (700u)
#define AXP_192_DCDC2_MAX_VOLTAGE             (2275u)

/**
 * Minimum and maximum voltages for LD02
 */
#define AXP_192_LDO2_MIN_VOLTAGE              (1800u)
#define AXP_192_LDO2_MAX_VOLTAGE              (3300u)
#define AXP_192_LDO2_OFFSET                   (1800u)
#define AXP_192_LDO2_FACTOR                   (100u)

/**
 * Minimum and maximum voltages for LD03
 */
#define AXP_192_LDO3_MIN_VOLTAGE             (1800u)
#define AXP_192_LDO3_MAX_VOLTAGE             (3300u)

/**
 * Maximum value of the timer.
 */
#define AXP_192_TIMER_MAX_VALUE              (127u)

#define AXP_192_REG12H_DCDC1_SWITCH_CONTROL_BIT           (0u)
#define AXP_192_REG12H_LDO2_SWITCH_CONTROL_BIT            (2u)
#define AXP_192_REG12H_LDO3_SWITCH_CONTROL_BIT            (3u)
#define AXP_192_REG12H_DCDC2_SWITCH_CONTROL_BIT           (4u)
#define AXP_192_REG12H_EXTEN_SWITCH_CONTROL_BIT           (6u)
/***************************************************************************************************
 * DECLARATIONS
 **************************************************************************************************/
static void Axp192_ReadI2cData();
static void Axp192_WriteI2cData(uint8_t*, size_t);
static void Axp192_ReadRegister(Axp192_RegisterType, uint8_t*);
static uint8_t Axp192_ReadRegister1Bit(Axp192_RegisterType registerAddress, uint8_t bitPosition);
static void Axp192_WriteRegister(Axp192_RegisterType, uint8_t);
static void Axp192_WriteRegister1Bit(Axp192_RegisterType registerAddress, uint8_t bitPosition, uint8_t value);
static void Axp192_UpdatePowerOutputControlRegister(Axp192_StateType, uint8_t);
static uint16_t Axp192_GetCurrentValue(Axp192_RegisterType highRegister, Axp192_RegisterType lowRegister);
static uint32_t Axp192_GetColumbMeterData(Axp192_RegisterType bit31To24Register, Axp192_RegisterType bit23To16Register, Axp192_RegisterType bit15To08Register, Axp192_RegisterType bit07To00Register);
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
  i2c_param_config(AXP_192_I2C_PORT, &Axp192_Configuration);
  i2c_driver_install(AXP_192_I2C_PORT, Axp192_Configuration.mode, 0, 0, ESP_INTR_FLAG_IRAM);
}

void Axp192_SetDcDc1State(Axp192_StateType state)
{
  Axp192_UpdatePowerOutputControlRegister(state, AXP_192_REG12H_DCDC1_SWITCH_CONTROL_BIT);
}

void Axp192_SetDcDc1Voltage(uint16_t voltage)
{
  if (voltage < AXP_192_DCDC1_MIN_VOLTAGE)
  {
    ESP_LOGE(__FUNCTION__, "Voltage below minimum voltage");
  }
  else if (voltage > AXP_192_DCDC1_MAX_VOLTAGE)
  {
    ESP_LOGE(__FUNCTION__, "Voltage beyond minimum voltage");
  }
  else
  {
    /* Calculate register value based on a resolution of 25mV per bit and 0.7V offset */
    voltage = (voltage - 700) / 25;
    Axp192_WriteRegister(Axp192_Dcdc1VoltageSettingRegister, (uint8_t)voltage);
  }
}

void Axp192_SetDcDc2State(Axp192_StateType state)
{
  Axp192_UpdatePowerOutputControlRegister(state, AXP_192_REG12H_DCDC2_SWITCH_CONTROL_BIT);
}

void Axp192_SetDcDc2Voltage(uint16_t voltage)
{
  if (voltage < AXP_192_DCDC2_MIN_VOLTAGE)
  {
    ESP_LOGE(__FUNCTION__, "Voltage below minimum voltage");
  }
  else if (voltage > AXP_192_DCDC2_MAX_VOLTAGE)
  {
    ESP_LOGE(__FUNCTION__, "Voltage beyond minimum voltage");
  }
  else
  {
    /* Calculate register value based on a resolution of 25mV per bit and 0.7V offset */
    voltage = (voltage - 700) / 25;
    Axp192_WriteRegister(Axp192_Dcdc2VoltageSettingRegister, (uint8_t)voltage);
  }
}

void Axp192_SetLdo2State(Axp192_StateType state)
{
  Axp192_UpdatePowerOutputControlRegister(state, AXP_192_REG12H_LDO2_SWITCH_CONTROL_BIT);
}

void Axp192_SetLdo2Voltage(uint16_t voltage)
{
  if (voltage < AXP_192_LDO2_MIN_VOLTAGE)
  {
    ESP_LOGE(__FUNCTION__, "Voltage below minimum voltage");
  }
  else if (voltage > AXP_192_LDO2_MAX_VOLTAGE)
  {
    ESP_LOGE(__FUNCTION__, "Voltage beyond minimum voltage");
  }
  else
  {
    uint8_t registerValue;

    Axp192_ReadRegister(Axp192_LDO2_3_OutputVoltageSettingRegister, &registerValue);

    /* Calculate register value based on a resolution of 100mV per bit and 1.8V offset */
    voltage = (voltage - 1800) / 100;
    voltage = (voltage << 4) & 0xF0;

    /* Mask out voltage value from register */
    registerValue &= 0x0F;

    /* Set new voltage value */
    registerValue |= voltage;
    Axp192_WriteRegister(Axp192_LDO2_3_OutputVoltageSettingRegister, registerValue);
  }
}

void Axp192_SetLdo3State(Axp192_StateType state)
{
  Axp192_UpdatePowerOutputControlRegister(state, AXP_192_REG12H_LDO3_SWITCH_CONTROL_BIT);
}

uint16_t Axp192_GetLdo3Voltage()
{
    uint8_t registerValue;
    uint16_t voltage;
    Axp192_ReadRegister(Axp192_LDO2_3_OutputVoltageSettingRegister, &registerValue);

    voltage = registerValue & 0x0F;
    voltage = AXP_192_LDO2_OFFSET + (AXP_192_LDO2_FACTOR * voltage);

    return voltage;
}

void Axp192_SetLdo3Voltage(uint16_t voltage)
{
  if (voltage < AXP_192_LDO3_MIN_VOLTAGE)
  {
    ESP_LOGE(__FUNCTION__, "Voltage below minimum voltage");
  }
  else if (voltage > AXP_192_LDO3_MAX_VOLTAGE)
  {
    ESP_LOGE(__FUNCTION__, "Voltage beyond minimum voltage");
  }
  else
  {
    uint8_t registerValue;

    Axp192_ReadRegister(Axp192_LDO2_3_OutputVoltageSettingRegister, &registerValue);

    /* Calculate register value based on a resolution of 100mV per bit and 1.8V offset */
    voltage = (voltage - AXP_192_LDO2_OFFSET) / AXP_192_LDO2_FACTOR;
    voltage &= 0x0F;

    /* Mask out voltage value from register */
    registerValue &= 0xF0;

    /* Set new voltage value */
    registerValue |= voltage;
    Axp192_WriteRegister(Axp192_LDO2_3_OutputVoltageSettingRegister, registerValue);
  }
}

void Axp192_SetExtenState(Axp192_StateType state)
{
  Axp192_UpdatePowerOutputControlRegister(state, AXP_192_REG12H_EXTEN_SWITCH_CONTROL_BIT);
}

uint16_t Axp192_GetBatteryVoltage()
{
  uint16_t result;
  uint8_t batteryVoltageLow;
  uint8_t batteryVoltageHigh;

  Axp192_ReadRegister(Axp192_BatteryVoltageLow4Bit, &batteryVoltageLow);
  Axp192_ReadRegister(Axp192_BatteryVoltageHigh8Bit, &batteryVoltageHigh);

  result = (((uint16_t)batteryVoltageHigh) << 4) | (batteryVoltageLow & 0x0F);

  /* Calculate physical value based on a resolution of 1.1mV per digit */
  result *= 11;
  result /= 10;
  return result;
}

uint16_t Axp192_GetBatteryChargeCurrent()
{
  return Axp192_GetCurrentValue(Axp192_BatteryChargeCurrentHigh8Bit, Axp192_BatteryChargeCurrentLow5Bit);
}

uint16_t Axp192_GetBatteryDischargeCurrent()
{
  return Axp192_GetCurrentValue(Axp192_BatteryDischargeCurrentHigh8Bit, Axp192_BatteryDischargeCurrentLow5Bit);
}

uint32_t Axp192_GetBatteryCharge()
{
  uint32_t chargeColumbMeterData = Axp192_GetColumbMeterData(
      Axp192_BatteryChargingCoulombMeterDataRegister31to24,
      Axp192_BatteryChargingCoulombMeterDataRegister21to16,
      Axp192_BatteryChargingCoulombMeterDataRegister15to08,
      Axp192_BatteryChargingCoulombMeterDataRegister07to00);
  uint32_t dischargeColumbMeterData = Axp192_GetColumbMeterData(
      Axp192_BatteryDischargeCoulombMeterDataRegister31to24,
      Axp192_BatteryDischargeCoulombMeterDataRegister21to16,
      Axp192_BatteryDischargeCoulombMeterDataRegister15to08,
      Axp192_BatteryDischargeCoulombMeterDataRegister07to00);

  return 65536 / 2 * (chargeColumbMeterData - dischargeColumbMeterData) / 3600 / Axp192_GetAdcSamplingRate();
}

Axp192_AdcSamplingRateType Axp192_GetAdcSamplingRate()
{
  uint8_t registerValue;
  Axp192_AdcSamplingRateType returnValue;
  Axp192_ReadRegister(Axp192_AdcSampleRateRegisterAndTsPinControlRegister, &registerValue);
  switch ((registerValue >> 6) & 0x03)
  {
    case 0:
      returnValue = Axp192_AdcSampelRate25Hz;
      break;
    case 1:
      returnValue = Axp192_AdcSampelRate50Hz;
      break;
    case 2:
      returnValue = Axp192_AdcSampelRate100Hz;
      break;
    case 3:
      returnValue = Axp192_AdcSampelRate200Hz;
      break;
  }

  return returnValue;
}

Axp192_StateType Axp192_GetChargeFunctionState()
{
  return (Axp192_StateType)Axp192_ReadRegister1Bit(Axp192_ChargeControlRegister1, 7);
}

Axp192_ChargeTargetVoltageType Axp192_GetChargeTargetVoltage()
{
  uint8_t registerValue;
  Axp192_ReadRegister(Axp192_ChargeControlRegister1, &registerValue);
  return (Axp192_StateType)((registerValue >> 5) & 0x03);
}

Axp192_PowerModeType Axp192_GetPowerMode()
{
  return (Axp192_PowerModeType)Axp192_ReadRegister1Bit(Axp192_PowerMode_ChargeStatusRegister, 1);
}

Axp192_StateType Axp192_GetPwronWakeupFunctionState()
{
  return (Axp192_StateType)Axp192_ReadRegister1Bit(Axp192_VoffShutdownVoltageSettingRegister, 3);
}

void Axp192_SetPwronWakeupFunctionState(Axp192_StateType state)
{
  Axp192_WriteRegister1Bit(Axp192_VoffShutdownVoltageSettingRegister, 3, state);
}

void Axp192_Shutdown()
{
  Axp192_WriteRegister1Bit(Axp192_ShutdownBatteryDetectionChargeLedControlRegister, 7, 1);
}

void Axp192_SetTimer(uint8_t minutes)
{
  if (minutes > AXP_192_TIMER_MAX_VALUE)
  {
    ESP_LOGE(__FUNCTION__, "Timout beyond maximum voltage");
  }
  else
  {
    Axp192_WriteRegister(Axp192_TimerControlRegister, minutes);
  }
}

static void Axp192_ReadRegister(Axp192_RegisterType registerAddress, uint8_t* buffer)
{
  uint8_t txData = registerAddress;
  Axp192_WriteI2cData(&txData, 1);
  Axp192_ReadI2cData(buffer, 1);
}

static void Axp192_WriteRegister(Axp192_RegisterType registerAddress, uint8_t data)
{
  uint8_t txData[2] = { registerAddress, data };
  Axp192_WriteI2cData(txData, 2);
}

static uint8_t Axp192_ReadRegister1Bit(Axp192_RegisterType registerAddress, uint8_t bitPosition)
{
  uint8_t registerValue;
 Axp192_ReadRegister(registerAddress, &registerValue);
 return ((registerValue >> bitPosition) & 0x01);
}

static void Axp192_WriteRegister1Bit(Axp192_RegisterType registerAddress, uint8_t bitPosition, uint8_t value)
{
  uint8_t registerValue;
  Axp192_ReadRegister(registerAddress, &registerValue);
  if (value == 0)
  {
    registerValue &= ~ (1 << bitPosition);
  }
  else if (value == 1)
  {
    registerValue |= (1 << bitPosition);
  }

  Axp192_WriteRegister(registerAddress, registerValue);
}

static void Axp192_ReadI2cData(uint8_t* buffer, size_t bufferLength)
{
  i2c_cmd_handle_t i2c_cmd_handle = i2c_cmd_link_create();
  ESP_ERROR_CHECK(i2c_master_start(i2c_cmd_handle));
  ESP_ERROR_CHECK(i2c_master_write_byte(i2c_cmd_handle, (AXP_192_SLAVE_ADDRESS << 1) | I2C_MASTER_READ, true));
  ESP_ERROR_CHECK(i2c_master_read_byte(i2c_cmd_handle, buffer, I2C_MASTER_NACK));
  ESP_ERROR_CHECK(i2c_master_stop(i2c_cmd_handle));
  ESP_ERROR_CHECK(i2c_master_cmd_begin(AXP_192_I2C_PORT, i2c_cmd_handle, 1000 / portTICK_RATE_MS));
  i2c_cmd_link_delete(i2c_cmd_handle);
}

static void Axp192_WriteI2cData(uint8_t* data, size_t dataLength)
{
  i2c_cmd_handle_t i2c_cmd_handle = i2c_cmd_link_create();
  ESP_ERROR_CHECK(i2c_master_start(i2c_cmd_handle));
  ESP_ERROR_CHECK(i2c_master_write_byte(i2c_cmd_handle, (AXP_192_SLAVE_ADDRESS << 1) | I2C_MASTER_WRITE, true));
  ESP_ERROR_CHECK(i2c_master_write(i2c_cmd_handle, data, dataLength, true));
  ESP_ERROR_CHECK(i2c_master_stop(i2c_cmd_handle));
  ESP_ERROR_CHECK(i2c_master_cmd_begin(AXP_192_I2C_PORT, i2c_cmd_handle, 5000 / portTICK_RATE_MS));
  i2c_cmd_link_delete(i2c_cmd_handle);
}

static void Axp192_UpdatePowerOutputControlRegister(Axp192_StateType state, uint8_t bit)
{
  Axp192_WriteRegister1Bit(Axp192_Dcdc1_3AndLDO2_3SwitchControlRegister, bit, state);
}

static uint16_t Axp192_GetCurrentValue(Axp192_RegisterType highRegister, Axp192_RegisterType lowRegister)
{
  uint16_t result;
  uint8_t highRegisterValue;
  uint8_t lowRegisterValue;
  Axp192_ReadRegister(highRegister, &highRegisterValue);
  Axp192_ReadRegister(lowRegister, &lowRegisterValue);
  /* Use only the lowest 5 bits */
  result = lowRegisterValue & 0x1F;
  result |= (((uint16_t)highRegisterValue) << 5);

  /* Calculate physical value based on a resolution of 0.5mA per digit */
  result /= 2;
  return result;
}

static uint32_t Axp192_GetColumbMeterData(Axp192_RegisterType bit31To24Register, Axp192_RegisterType bit23To16Register, Axp192_RegisterType bit15To08Register, Axp192_RegisterType bit07To00Register)
{
  uint8_t byte0, byte1, byte2, byte3;
  Axp192_ReadRegister(bit31To24Register, &byte0);
  Axp192_ReadRegister(bit23To16Register, &byte1);
  Axp192_ReadRegister(bit15To08Register, &byte2);
  Axp192_ReadRegister(bit07To00Register, &byte3);

  return ((((uint32_t)byte0) << 24) | (((uint32_t)byte1) << 16) | (((uint32_t)byte2) << 8) | byte3);
}
