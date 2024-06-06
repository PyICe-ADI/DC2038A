/****************************************************************************
 * DC2038A SMBus Common Command Structure                                   *
 * Steve Martin                                                             *
 * June 5, 2024                                                             *
 ****************************************************************************/
#ifndef DC2038A_SMBUS_COMND_STRUCT_H
#define DC2038A_SMBUS_COMND_STRUCT_H

/****************************************************************************
 * Straight Through SMBus Transactions and Alternate Commands               *
 ****************************************************************************/
#define SMBUS_QUICK_COMAND          1 // The R/W bit is the data
#define SMBUS_SEND_BYTE             2 // Byte only
#define SMBUS_RECEIVE_BYTE          3 // Byte only
#define SMBUS_WRITE_REGISTER        4 // Byte and Word, With and without PEC
#define SMBUS_READ_REGISTER         5 // Byte and Word, With and without PEC
#define SMBUS_PROCESS_CALL          6 // With and without PEC
#define SMBUS_BLOCK_WRITE           7 // With and without PEC
#define SMBUS_BLOCK_READ            8 // With and without PEC
#define SET_REGISTER_LIST           21
#define READ_REGISTER_LIST          22
#define ENABLE_STREAM_MODE          23
#define DISABLE_STREAM_MODE         24
#define WRITE_REGISTER_LIST         25
#define SET_REG_LIST_AND_READ_LIST  26
#define SET_REG_LIST_AND_STREAM     27

/****************************************************************************
 * Payload Byte Positions                                                   *
 ****************************************************************************/
//┌───────────────────────┬───────┬──────────────┬─────────┬─────────────────────────┬───────┬───────┬─────────────┐
//│ TRANSACTION / COMMAND │ ADDR7 │ COMMAND CODE │ USE PEC │ DATA_SIZE(BITS 8 or 16) │ REG 1 │ REG 2 │ REG 3   ... │
//└───────────────────────┴───────┴──────────────┴─────────┴─────────────────────────┴───────┴───────┴─────────────┘
#define TRANSACTION_TYPE        0 // See command table above
#define ADDR7                   1 // Target 7-bit address
#define COMMAND_CODE            2 // Targetlocation of data to be sent (unused for "list" commands)
#define USE_PEC                 3 // Expect 1 or 0
#define DATA_SIZE               4 // In Bits not Bytes (8 or 16)
#define SMBUS_COMMAND_LEN       5
#define START_OF_SMBUS_DATA_IN  SMBUS_COMMAND_LEN // Data goes from here to the remainder of the payload
#define START_OF_DATA_OUT       0

#endif