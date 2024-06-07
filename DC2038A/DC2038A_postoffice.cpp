/****************************************************************************
 * DC2038A Post Office                                                      *
 * Steve Martin                                                             *
 * June 5, 2024                                                             *
 ****************************************************************************/
#include "DC2038A_postoffice.h"

/****************************************************************************
 * Create Mailboxes                                                         *
 ****************************************************************************/
Mailbox<SMBUS_INBOX_SIZE, SMBUS_OUTBOX_SIZE>        smbus_mailbox       = {.my_id=SMBUS_ADDRESS,    .to_id=0, .from_id=0, .inbox_msg_size=0, .outbox_msg_size=0, .inbox_status=PACKET_ABSENT, .outbox_status=PACKET_ABSENT, .inbox_overflow=false, .inbox={0}, .outbox={0}};
Mailbox<IDENTIFY_INBOX_SIZE, IDENTIFY_OUTBOX_SIZE>  identify_mailbox    = {.my_id=IDENTIFY_ADDRESS, .to_id=0, .from_id=0, .inbox_msg_size=0, .outbox_msg_size=0, .inbox_status=PACKET_ABSENT, .outbox_status=PACKET_ABSENT, .inbox_overflow=false, .inbox={0}, .outbox={0}};
Mailbox<SMBALERT_INBOX_SIZE, SMBALERT_OUTBOX_SIZE>  smbalert_mailbox    = {.my_id=SMBALERT_ADDRESS, .to_id=0, .from_id=0, .inbox_msg_size=0, .outbox_msg_size=0, .inbox_status=PACKET_ABSENT, .outbox_status=PACKET_ABSENT, .inbox_overflow=false                         };
/****************************************************************************
 *                                                                          *
 ****************************************************************************/

/****************************************************************************
 * Process incoming and outgoing mail                                       *
 ****************************************************************************/
void DC2038A_process_mail()
{
   /********************************
    * Incoming Mail                *
    ********************************/
    if(labcomm_packet.available)
        switch(labcomm_packet.destination_id)
        {
            case SMBUS_ADDRESS:     smbus_mailbox.get_packet(&labcomm_packet);      break;
            case IDENTIFY_ADDRESS:  identify_mailbox.get_packet(&labcomm_packet);   break;
            case SMBALERT_ADDRESS:  smbalert_mailbox.get_packet(&labcomm_packet);   break;
        }
   /********************************
    * Outgoing Mail                *
    ********************************/
    if(smbus_mailbox.outbox_status == PACKET_PRESENT)       smbus_mailbox.send_packet();
    if(labcomm_mailbox.outbox_status == PACKET_PRESENT)     labcomm_mailbox.send_packet();
    if(identify_mailbox.outbox_status == PACKET_PRESENT)    identify_mailbox.send_packet();
    if(smbalert_mailbox.outbox_status == PACKET_PRESENT)    smbalert_mailbox.send_packet();
}