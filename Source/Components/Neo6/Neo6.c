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
#include "Neo6.h"
#include "Neo6_Cfg.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "string.h"
/***************************************************************************************************
 * DEFINES
 **************************************************************************************************/
#define NEO6_UART_MESSAGE_SYNC_CHAR_1             (0xB5)
#define NEO6_UART_MESSAGE_SYNC_CHAR_2             (0x62)

/**
 * Position of the payload start in a UBX message.
 */
#define NEO6_UART_MESSAGE_PAYLOAD_OFFSET          (0x06)

#define NEO6_CFG_PORT_MESSAGE_CLASS               (0x06)
#define NEO6_CFG_PORT_MESSAGE_ID                  (0x00)
#define NEO6_CFG_PORT_RESPONSE_PAYLOAD_LENGTH      (20u)

#define NEO6_CFG_RST_MESSAGE_CLASS                (0x06)
#define NEO6_CFG_RST_MESSAGE_ID                   (0x04)
#define NEO6_CFG_RST_PAYLOAD_LENGTH                 (2u)

#define NEO6_NAV_POSLLH_MESSAGE_CLASS             (0x01)
#define NEO6_NAV_POSLLH_MESSAGE_ID                (0x02)
#define NEO6_NAV_POSLLH_PAYLOAD_LENGTH             (28u)

#define NEO6_BUFFER_TO_32BIT_VALUE(buffer, start) \
  ((buffer[start + 0]) | (buffer[start + 1] << 8) | (buffer[start + 2] << 16) | (buffer[start + 3] << 24))
/***************************************************************************************************
 * TYPES
 **************************************************************************************************/

typedef struct
{
  uint8_t MessageClass;
  uint8_t MessageId;
  uint16_t PayloadLength;
  uint8_t* Payload;
} Neo6_UbxMessageType;
/***************************************************************************************************
 * DECLARATIONS
 **************************************************************************************************/
static void Neo6_InitInternal(const uart_config_t* config);
static void Neo6_MemCopy(uint8_t* source, uint8_t* destination, size_t count);
static Neo6_StatusType Neo6_UartReceive(Neo6_UbxMessageType* message);
static Neo6_StatusType Neo6_UartTransmit(Neo6_UbxMessageType* message);
static uint16_t Neo6_GetMessageLength(Neo6_UbxMessageType* message);
static void Neo6_CalculateUbxChecksum(uint8_t* buffer, uint16_t bufferLength, uint8_t* ckA, uint8_t* ckB);

/***************************************************************************************************
 * CONSTANTS
 **************************************************************************************************/

/***************************************************************************************************
 * VARIABLES
 **************************************************************************************************/

/***************************************************************************************************
 * IMPLEMENTATION
 **************************************************************************************************/
void Neo6_InitMemory()
{
}

void Neo6_Init()
{
  uart_config_t uart_config = {
      .baud_rate = 9600,
      .data_bits = UART_DATA_8_BITS,
      .parity = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
  };

  Neo6_InitInternal(&uart_config);

  /* UART1 configuration: Switch to 115200 baud, disabled cyclic NMEA output */
  const char* command = "$PUBX,41,1,0007,0001,115200,0*1A\r\n";
  uart_write_bytes(NEO6_UART_PERIPHERAL, command, strlen(command));
  uart_wait_tx_done(NEO6_UART_PERIPHERAL, 200 / portTICK_PERIOD_MS);

  /*Re-initialize UART with 115200 baud */
  uart_driver_delete(NEO6_UART_PERIPHERAL);
  uart_config.baud_rate = 115200;
  Neo6_InitInternal(&uart_config);
}

Neo6_StatusType Neo6_GetGeodeticPositionSolution(Neo6_GeodeticPositionSolutionType* geodeticPositionSolution)
{
  Neo6_StatusType result = Neo6_Failed;
  uint8_t buffer[NEO6_NAV_POSLLH_PAYLOAD_LENGTH];
  Neo6_UbxMessageType message;
  message.PayloadLength = 0;
  message.MessageClass = NEO6_NAV_POSLLH_MESSAGE_CLASS;
  message.MessageId = NEO6_NAV_POSLLH_MESSAGE_ID;
  message.Payload = buffer;

  if (Neo6_UartTransmit(&message) == Neo6_Success)
  {
    message.PayloadLength = NEO6_NAV_POSLLH_PAYLOAD_LENGTH;
    if (Neo6_UartReceive(&message) == Neo6_Success)
    {
      geodeticPositionSolution->TimeOfWeek = NEO6_BUFFER_TO_32BIT_VALUE(buffer, 0);
      geodeticPositionSolution->Longitude =  NEO6_BUFFER_TO_32BIT_VALUE(buffer, 4);
      geodeticPositionSolution->Latitude =  NEO6_BUFFER_TO_32BIT_VALUE(buffer, 8);
      geodeticPositionSolution->HeightAboveEllipsoid = NEO6_BUFFER_TO_32BIT_VALUE(buffer, 12);
      geodeticPositionSolution->HeightAboveMeanSeaLevel = NEO6_BUFFER_TO_32BIT_VALUE(buffer, 16);
      geodeticPositionSolution->HorizontalAccuracyEstimate = NEO6_BUFFER_TO_32BIT_VALUE(buffer, 20);
      geodeticPositionSolution->VertictalAccuracyEstimate = NEO6_BUFFER_TO_32BIT_VALUE(buffer, 24);
      result = Neo6_Success;
    }
  }

  return result;
}

Neo6_StatusType Neo6_SetOutputFormat(Neo6_OutputFormat format, Neo6_StateType state)
{
  Neo6_StatusType result = Neo6_Failed;
  uint8_t buffer[NEO6_CFG_PORT_RESPONSE_PAYLOAD_LENGTH];
  Neo6_UbxMessageType message;
  message.PayloadLength = 0;
  message.MessageClass = NEO6_CFG_PORT_MESSAGE_CLASS;
  message.MessageId = NEO6_CFG_PORT_MESSAGE_ID;
  message.Payload = buffer;

  if (Neo6_UartTransmit(&message) == Neo6_Success)
  {
    message.PayloadLength = NEO6_CFG_PORT_RESPONSE_PAYLOAD_LENGTH;
    if (Neo6_UartReceive(&message) == Neo6_Success)
    {
      if (state == Neo6_On)
      {
        message.Payload[14] |= (1 << format);
      }
      else if (state == Neo6_Off)
      {
        message.Payload[14] &= (~(1 << format));
      }

      result = Neo6_UartTransmit(&message);
    }
  }

  return result;
}

void Neo6_ApplyReset(Neo6_ResetType resetType)
{
  uint8_t buffer[NEO6_CFG_RST_PAYLOAD_LENGTH];
  Neo6_UbxMessageType message;
  message.PayloadLength = 0;
  message.MessageClass = NEO6_CFG_RST_MESSAGE_CLASS;
  message.MessageId = NEO6_CFG_RST_MESSAGE_ID;
  message.Payload = buffer;

  message.Payload[2] = resetType;

  Neo6_UartTransmit(&message);
}

static void Neo6_InitInternal(const uart_config_t* config)
{
  /* Configure UART parameters */
  ESP_ERROR_CHECK(uart_param_config(NEO6_UART_PERIPHERAL, config));

  /* Set UART pins */
  ESP_ERROR_CHECK(uart_set_pin(NEO6_UART_PERIPHERAL, NEO6_UART_TX_PIN, NEO6_UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

  /* Install UART driver using an event queue here */
  ESP_ERROR_CHECK(uart_driver_install(NEO6_UART_PERIPHERAL, NEO6_UART_BUFFER_SIZE, 0, 0, NULL, 0));

  ESP_ERROR_CHECK(uart_flush(NEO6_UART_PERIPHERAL));
}

static void Neo6_MemCopy(uint8_t* source, uint8_t* destination, size_t count)
{
  for (uint32_t position = 0; position < count; position++)
  {
    destination[position] = source[position];
  }
}

static Neo6_StatusType Neo6_UartReceive(Neo6_UbxMessageType* message)
{
  static uint8_t Neo6_ReceiveBuffer[1024];
  Neo6_StatusType result = Neo6_Failed;
  uint16_t bufferPosition = 0;

  uint16_t receiveDataLength = Neo6_GetMessageLength(message);

  if (receiveDataLength <= sizeof (Neo6_ReceiveBuffer))
  {
    if (uart_read_bytes(NEO6_UART_PERIPHERAL, Neo6_ReceiveBuffer, receiveDataLength, 100) != receiveDataLength)
    {
      ESP_LOGE(__FUNCTION__, "Wrong number of bytes received");
    }
    else if (Neo6_ReceiveBuffer[bufferPosition++] != NEO6_UART_MESSAGE_SYNC_CHAR_1)
    {
      ESP_LOGE(__FUNCTION__, "Wrong Sync char 1");
      /*  */
    }
    else if (Neo6_ReceiveBuffer[bufferPosition++] != NEO6_UART_MESSAGE_SYNC_CHAR_2)
    {
      ESP_LOGE(__FUNCTION__, "Wrong Sync char 2");
      /*  */
    }
    else if (Neo6_ReceiveBuffer[bufferPosition++] != message->MessageClass)
    {
      ESP_LOGE(__FUNCTION__, "Wrong Message Class");
      /*  */
    }
    else if (Neo6_ReceiveBuffer[bufferPosition++] != message->MessageId)
    {
      ESP_LOGE(__FUNCTION__, " Wrong Message ID");
    }
    else
    {
      uint16_t payloadLengLowByte = Neo6_ReceiveBuffer[bufferPosition++];
      uint16_t payloadLengthHighByte = Neo6_ReceiveBuffer[bufferPosition++];
      uint16_t payloadLength = payloadLengLowByte | (payloadLengthHighByte << 8);
      if (payloadLength == message->PayloadLength)
      {
        uint8_t chkA;
        uint8_t chkB;
        Neo6_CalculateUbxChecksum(
                &(Neo6_ReceiveBuffer[2]),               /* Start at byte 2, sync chars are not included in checksum */
                message->PayloadLength + 4,             /* 4 bytes for message class, message ID and length filed */
                &chkA,
                &chkB);
        if ((chkA == Neo6_ReceiveBuffer[receiveDataLength - 2]) && (chkB == Neo6_ReceiveBuffer[receiveDataLength - 1]))
        {
          Neo6_MemCopy(&(Neo6_ReceiveBuffer[NEO6_UART_MESSAGE_PAYLOAD_OFFSET]), message->Payload, message->PayloadLength);
          result = Neo6_Success;
        }
      }
    }
  }

  return result;
}

static Neo6_StatusType Neo6_UartTransmit(Neo6_UbxMessageType* message)
{
  static uint8_t Neo6_TransmitBuffer[1024];
  Neo6_StatusType result = Neo6_Failed;
  uint16_t bufferPosition = 0;

  uint16_t transmitDataLength = Neo6_GetMessageLength(message);
  if (transmitDataLength <= sizeof(Neo6_TransmitBuffer))
  {
    Neo6_TransmitBuffer[bufferPosition] = NEO6_UART_MESSAGE_SYNC_CHAR_1;
    bufferPosition++;

    Neo6_TransmitBuffer[bufferPosition] = NEO6_UART_MESSAGE_SYNC_CHAR_2;
    bufferPosition++;

    Neo6_TransmitBuffer[bufferPosition] = message->MessageClass;
    bufferPosition++;

    Neo6_TransmitBuffer[bufferPosition] = message->MessageId;
    bufferPosition++;

    /* Length filed is little endian, low byte first */
    Neo6_TransmitBuffer[bufferPosition] = (uint8_t)(message->PayloadLength & 0xFF);
    bufferPosition++;

    Neo6_TransmitBuffer[bufferPosition] = (uint8_t)((message->PayloadLength >> 8 ) & 0xFF);
    bufferPosition++;

    Neo6_MemCopy(message->Payload, &(Neo6_TransmitBuffer[NEO6_UART_MESSAGE_PAYLOAD_OFFSET]), message->PayloadLength);

    Neo6_CalculateUbxChecksum(
        &(Neo6_TransmitBuffer[2]),               /* Start at byte 2, sync chars are not included in checksum */
        message->PayloadLength + 4,             /* 4 bytes for message class, message ID and length filed */
        &(Neo6_TransmitBuffer[transmitDataLength - 2]),
        &(Neo6_TransmitBuffer[transmitDataLength - 1]));

    if (uart_write_bytes(NEO6_UART_PERIPHERAL, (const char*)Neo6_TransmitBuffer, transmitDataLength) == transmitDataLength)
    {
      result = Neo6_Success;
    }
  }

  return result;
}

static uint16_t Neo6_GetMessageLength(Neo6_UbxMessageType* message)
{
  /* 2 bytes Sync, 1 byte message class, 1 byte message ID, 2 bytes length filed, 2 bytes checksum */
  return message->PayloadLength + 2 + 1 + 1 + 2 + 2;
}

static void Neo6_CalculateUbxChecksum(uint8_t* buffer, uint16_t bufferLength, uint8_t* ckA, uint8_t* ckB)
{
  *ckA = 0;
  *ckB = 0;

  /* Calculate checksum */
  for (uint16_t bufferPosition = 0; bufferPosition < bufferLength; bufferPosition++)
  {
    *ckA += buffer[bufferPosition];
    *ckB += *ckA;
  }
}
