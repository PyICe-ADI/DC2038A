/****************************************************************************
 * DC2038A LTC4162 SMBAlert Pin                                             *
 * Steve Martin                                                             *
 * June 5, 2024                                                             *
 ****************************************************************************/
#include "DC2038A_smbalert_pin.h"

/****************************************************************************
 * Get the SMBALERT pin state                                               *
 ****************************************************************************/
void DC2038A_get_smbalert_pin()
{
    if (smbalert_mailbox.inbox_status == PACKET_PRESENT)
    {
        smbalert_mailbox.to_id = smbalert_mailbox.from_id;
        smbalert_mailbox.outbox[0] = digitalRead(SMBALERTPIN);
        smbalert_mailbox.outbox_msg_size = SMBALERT_OUTBOX_SIZE;
        smbalert_mailbox.inbox_status = PACKET_ABSENT;
        smbalert_mailbox.outbox_status = PACKET_PRESENT;
    }
}