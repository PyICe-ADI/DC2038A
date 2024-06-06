/****************************************************************************
 * DC2038A Identify Oneself                                                 *
 * Steve Martin                                                             *
 * June 5, 2024                                                             *
 ****************************************************************************/
#include "DC2038A_identify.h"
#include <FlashStorage_SAMD.h>
// #include <FlashStorage_SAMD.h> shouldn't be in here.
// It should be in this file's .h file but it's a source file not really a .h and it causes a terminal linker error.
// See the comment in the GitHub:
// https://github.com/khoih-prog/FlashStorage_SAMD/blob/master/src/FlashStorage_SAMD.h
// The .hpp contains only definitions, and can be included as many times as necessary, without `Multiple Definitions` Linker Error
// The .h contains implementations, and can be included only in main(), .ino with setup() to avoid `Multiple Definitions` Linker Error

/* Command Menu */
#define QUERY                  '?'
#define WRITE_SCRATCHPAD        1
#define READ_SCRATCHPAD         2
#define GET_SERNUM              3

/* Other Enums */
#define COMMAND_BYTE            0
#define SERNUM_START_OF_DATA    1

typedef struct {
  uint8_t size;
  char message[SCRATCHPAD_SIZE];
} scratchpad_t;

FlashStorage(flash_storage, scratchpad_t);

/****************************************************************************
 * Identify Menu                                                            *
 ****************************************************************************/
void DC2038A_identify()
{
    if (identify_mailbox.inbox_status == PACKET_PRESENT)
    {
        switch(identify_mailbox.inbox[COMMAND_BYTE])
        {
            case QUERY:             send_identity();    break;
            case WRITE_SCRATCHPAD:  write_scratchpad(); break;
            case READ_SCRATCHPAD:   read_scratchpad();  break;
            case GET_SERNUM:        get_serialnum();    break;
        }
        identify_mailbox.inbox_status = PACKET_ABSENT;
    }
}
/****************************************************************************
 * Send Identity                                                            *
 ****************************************************************************/
void send_identity()
{
    memcpy(identify_mailbox.outbox, IDENTIFICATION, ID_STRING_MESSAGE_SIZE);
    identify_mailbox.to_id = identify_mailbox.from_id;
    identify_mailbox.outbox_msg_size = ID_STRING_MESSAGE_SIZE;
    identify_mailbox.outbox_status = PACKET_PRESENT;
}
/****************************************************************************
 * Write the scratchpad                                                     *
 ****************************************************************************/
void write_scratchpad()
{
    scratchpad_t scratchpad;
    scratchpad.size = identify_mailbox.inbox_msg_size - SERNUM_START_OF_DATA;
    memcpy(&scratchpad.message, &identify_mailbox.inbox[SERNUM_START_OF_DATA], identify_mailbox.inbox_msg_size - SERNUM_START_OF_DATA);
    flash_storage.write(scratchpad);
}
/****************************************************************************
 * Retreive the Scratchpad                                                  *
 ****************************************************************************/
void read_scratchpad()
{
    scratchpad_t scratchpad;
    flash_storage.read(scratchpad);
    identify_mailbox.to_id = identify_mailbox.from_id;
    identify_mailbox.outbox_msg_size = scratchpad.size;
    memcpy(identify_mailbox.outbox, scratchpad.message, scratchpad.size);
    identify_mailbox.outbox_status = PACKET_PRESENT;
}
/****************************************************************************
 * Retreive the Serial Number                                               *
 ****************************************************************************/
void get_serialnum()
{
//  https://microchip.my.site.com/s/article/Reading-unique-serial-number-on-SAM-D20-SAM-D21-SAM-R21-devices
//  https://gist.github.com/mgk/c9ec87436d2d679e5d08
    
    volatile uint32_t word0, word1, word2, word3;
    volatile uint32_t *ptr1 = (volatile uint32_t *)0x0080A00C;
    word0 = *ptr1;
    volatile uint32_t *ptr = (volatile uint32_t *)0x0080A040;
    word1 = *ptr;
    ptr++;
    word2 = *ptr;
    ptr++;
    word3 = *ptr;

    identify_mailbox.outbox[15-0]  = (word0 & 0x000000ff)   >> 0;
    identify_mailbox.outbox[15-1]  = (word0 & 0x0000ff00)   >> 8;
    identify_mailbox.outbox[15-2]  = (word0 & 0x00ff0000)   >> 12;
    identify_mailbox.outbox[15-3]  = (word0 & 0xff000000)   >> 16;
    identify_mailbox.outbox[15-4]  = (word1 & 0x000000ff)   >> 0;
    identify_mailbox.outbox[15-5]  = (word1 & 0x0000ff00)   >> 8;
    identify_mailbox.outbox[15-6]  = (word1 & 0x00ff0000)   >> 12;
    identify_mailbox.outbox[15-7]  = (word1 & 0xff000000)   >> 16;
    identify_mailbox.outbox[15-8]  = (word2 & 0x000000ff)   >> 0;
    identify_mailbox.outbox[15-9]  = (word2 & 0x0000ff00)   >> 8;
    identify_mailbox.outbox[15-10] = (word2 & 0x00ff0000)   >> 12;
    identify_mailbox.outbox[15-11] = (word2 & 0xff000000)   >> 16;
    identify_mailbox.outbox[15-12] = (word3 & 0x000000ff)   >> 0;
    identify_mailbox.outbox[15-13] = (word3 & 0x0000ff00)   >> 8;
    identify_mailbox.outbox[15-14] = (word3 & 0x00ff0000)   >> 12;
    identify_mailbox.outbox[15-15] = (word3 & 0xff000000)   >> 16;

    identify_mailbox.to_id = identify_mailbox.from_id;
    identify_mailbox.outbox_msg_size = 16;
    identify_mailbox.outbox_status = PACKET_PRESENT;
}