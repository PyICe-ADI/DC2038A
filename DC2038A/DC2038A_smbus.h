/****************************************************************************
 * DC2038A SMBus Phy                                                        *
 * Steve Martin                                                             *
 * June 5, 2024                                                             *
 ****************************************************************************/
#ifndef DC2038A_SMBUS_H
#define DC2038A_SMBUS_H

#include <Wire.h>
#include <Arduino.h>    // Needed for delayMicroseconds()
#include "pec.h"

/****************************************************************************
 * Wire Libray .endTransmission Return Codes                                 *
 ****************************************************************************/
#define WIRE_END_SUCCESS            0   // success.
#define WIRE_END_BUFFER_OVERFLOW    1   // data too long to fit in transmit buffer.
#define WIRE_END_NACK_ON_ADDRESS    2   // received NACK on transmit of address.
#define WIRE_END_NACK_ON_DATA       3   // received NACK on transmit of data.
#define WIRE_END_OTHER_ERROR        4   // other error.
#define WIRE_END_TIMEOUT            5   // timeout.

/****************************************************************************
 * One Hot Return Byte Error Codes (Coordinated with PyICe)                 *
 ****************************************************************************/
#define SMBUS_SUCCESS           0
#define SMBUS_NACK_ON_ADDRESS   1
#define SMBUS_NACK_ON_DATA      2
#define SMBUS_PEC_VALUE_ERROR   4
#define SMBUS_SMBUS_TIMEOUT     8
#define SMBUS_BUFFER_OVERFLOW   16
#define SMBUS_UNKNOWN_ERROR     32 // Catch all, includes timeout.

/****************************************************************************
 * Other ENUMS                                                              *
 ****************************************************************************/
#define BYTE_SIZE 8     // Bits
#define WORD_SIZE 16    // Bits

typedef struct {
        uint8_t status;
        uint8_t lo_byte;
        uint8_t hi_byte;
}       SMBUS_reply;

SMBUS_reply read_register(uint8_t address, uint8_t command_code, uint8_t use_pec, uint8_t data_size);
SMBUS_reply write_register(uint8_t address, uint8_t command_code, uint8_t use_pec, uint8_t data_size, uint8_t lo_byte, uint8_t hi_byte);

#endif