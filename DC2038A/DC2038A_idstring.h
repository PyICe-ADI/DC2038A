/****************************************************************************
 * DC2038A ID String                                                        *
 * Steve Martin                                                             *
 * June 5, 2024                                                             *
 ****************************************************************************/
#ifndef DC2038A_IDSTRING_H
#define DC2038A_IDSTRING_H
#include <avr/pgmspace.h>

const PROGMEM char IDENTIFICATION[] = "DC2038A LTC4162 Demo Board REV: 0.0";

#define SCRATCHPAD_SIZE         255
#define ID_STRING_MESSAGE_SIZE  sizeof(IDENTIFICATION) - 1 // Omit the string's null character
#define IDENTIFY_PAYLOAD_SIZE   (SCRATCHPAD_SIZE >= ID_STRING_MESSAGE_SIZE) ? SCRATCHPAD_SIZE : ID_STRING_MESSAGE_SIZE

#endif 