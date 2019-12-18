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

#define AXP_192_REG12H_DCDC1_SWITCH_CONTROL_BIT           (1u << 0u)
#define AXP_192_REG12H_LDO2_SWITCH_CONTROL_BIT            (1u << 2u)
#define AXP_192_REG12H_LDO3_SWITCH_CONTROL_BIT            (1u << 3u)
#define AXP_192_REG12H_DCDC2_SWITCH_CONTROL_BIT           (1u << 4u)
#define AXP_192_REG12H_EXTEN_SWITCH_CONTROL_BIT           (1u << 6u)
/***************************************************************************************************
 * DECLARATIONS
 **************************************************************************************************/
static void Axp192_ReadI2cData();
static void Axp192_WriteI2cData(uint8_t*, size_t);
static void Axp192_ReadRegister(Axp192_RegisterType, uint8_t*);
static void Axp192_WriteRegister(Axp192_RegisterType, uint8_t);
static void Axp192_UpdatePowerOutputControlRegister(Axp192_StateType, uint8_t);
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
    ESP_LOGE(__FUNCTION__, "Voltage below minimum voltage");
  }
  else
  {
    /* Calculate register value based on a resolution of 25mV per bit and 0.7V offset */
    voltage = (voltage - 700) / 25;
    Axp192_WriteRegister(Dcdc1VoltageSettingRegister, (uint8_t)voltage);
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
    ESP_LOGE(__FUNCTION__, "Voltage below minimum voltage");
  }
  else
  {
    /* Calculate register value based on a resolution of 25mV per bit and 0.7V offset */
    voltage = (voltage - 700) / 25;
    Axp192_WriteRegister(Dcdc2VoltageSettingRegister, (uint8_t)voltage);
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
    ESP_LOGE(__FUNCTION__, "Voltage below minimum voltage");
  }
  else
  {
    uint8_t registerValue;

    Axp192_ReadRegister(LDO2_3_OutputVoltageSettingRegister, &registerValue);

    /* Calculate register value based on a resolution of 100mV per bit and 1.8V offset */
    voltage = (voltage - 1800) / 100;
    voltage = (voltage << 4) & 0xF0;

    /* Mask out voltage value from register */
    registerValue &= 0x0F;

    /* Set new voltage value */
    registerValue |= voltage;
    Axp192_WriteRegister(LDO2_3_OutputVoltageSettingRegister, registerValue);
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
    Axp192_ReadRegister(LDO2_3_OutputVoltageSettingRegister, &registerValue);

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
    ESP_LOGE(__FUNCTION__, "Voltage below minimum voltage");
  }
  else
  {
    uint8_t registerValue;

    Axp192_ReadRegister(LDO2_3_OutputVoltageSettingRegister, &registerValue);

    /* Calculate register value based on a resolution of 100mV per bit and 1.8V offset */
    voltage = (voltage - AXP_192_LDO2_OFFSET) / AXP_192_LDO2_FACTOR;
    voltage &= 0x0F;

    /* Mask out voltage value from register */
    registerValue &= 0xF0;

    /* Set new voltage value */
    registerValue |= voltage;
    Axp192_WriteRegister(LDO2_3_OutputVoltageSettingRegister, registerValue);
  }
}

void Axp192_SetExtenState(Axp192_StateType state)
{
  Axp192_UpdatePowerOutputControlRegister(state, AXP_192_REG12H_EXTEN_SWITCH_CONTROL_BIT);
}

uint16_t Axp192_GetBatteryVoltage()
{
  uint8_t batteryVoltageLow;
  uint8_t batteryVoltageHigh;

  Axp192_ReadRegister(BatteryVoltageLow4Bit, &batteryVoltageLow);
  Axp192_ReadRegister(BatteryVoltageHigh8Bit, &batteryVoltageHigh);

  return (((uint16_t)batteryVoltageHigh) << 4) | (batteryVoltageLow & 0x0F);

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
  uint8_t registerValue;
  Axp192_ReadRegister(Dcdc1_3AndLDO2_3SwitchControlRegister, &registerValue);
  if (state == Axp192_Off)
  {
    registerValue &= ~ bit;
  }
  else if (state == Axp192_On)
  {
    registerValue |= bit;
  }

  Axp192_WriteRegister(Dcdc1_3AndLDO2_3SwitchControlRegister, registerValue);
}
