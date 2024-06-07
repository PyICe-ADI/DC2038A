/****************************************************************************
 * DC2038A Post Office                                                      *
 * Steve Martin                                                             *
 * June 5, 2024                                                             *
 ****************************************************************************/
#ifndef DC2038A_POSTOFFICE_H
#define DC2038A_POSTOFFICE_H

#include "labcomm.h"                    // Gets incoming_packet and MAX_BUFFER_SIZE_MESSAGE_SIZE
#include "DC2038A_idstring.h"           // Gets IDENTIFICATION string
#include "DC2038A_smbus_comnd_struct.h" // Gets start of SMBus register as proxy for command size.

/****************************************************************************
 * Mail Box Addresses                                                       *
 ****************************************************************************/
#define PYICE_GUI_ADDRESS   1   // Must match expected Python addresses
#define SMBUS_ADDRESS       2   // Must match expected Python addresses
#define IDENTIFY_ADDRESS    3   // Must match expected Python addresses
#define SMBALERT_ADDRESS    4   // Must match expected Python addresses

/****************************************************************************
 * Mail Box Sizes                                                           *
 ****************************************************************************/
#define LABCOMM_INBOX_SIZE          0
#define LABCOMM_OUTBOX_SIZE         MAX_BUFFER_SIZE_MESSAGE_SIZE

#define IDENTIFY_INBOX_SIZE         1 + IDENTIFY_PAYLOAD_SIZE
#define IDENTIFY_OUTBOX_SIZE        IDENTIFY_PAYLOAD_SIZE

#define SMBUS_INBOX_SIZE            START_OF_SMBUS_DATA_IN + 3 * 256    // Write list will have: [ADDR|DATA|ADDR|DATA....] where size(data) can be 16
#define SMBUS_OUTBOX_SIZE           3 * 256                             // Supports read word plus 1 status byte per read word

#define SMBALERT_INBOX_SIZE     0
#define SMBALERT_OUTBOX_SIZE    1

/****************************************************************************
 * Mailbox Externs for the clients to find                                  *
 ****************************************************************************/
extern Mailbox<SMBUS_INBOX_SIZE,    SMBUS_OUTBOX_SIZE>      smbus_mailbox;
extern Mailbox<IDENTIFY_INBOX_SIZE, IDENTIFY_OUTBOX_SIZE>   identify_mailbox;
extern Mailbox<SMBALERT_INBOX_SIZE, SMBALERT_OUTBOX_SIZE>   smbalert_mailbox;

void DC2038A_process_mail();

#endif