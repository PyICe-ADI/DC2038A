/****************************************************************************
 * Labcomm                                                                  *
 * Steve Martin                                                             *
 * June 1, 2023                                                             *
 ****************************************************************************/
#include "labcomm.h"

uint8_t source_id_byte_count        = 0;
uint8_t receiver_id_byte_count      = 0;
uint8_t payload_size_byte_count     = 0;
uint8_t endamble_byte_count         = 0;
unsigned long payload_index         = 0;
uint8_t parser_state                = IDLE;
uint8_t preamble_buffer[PREAMBLE_SIZE];
uint8_t endamble_buffer[ENDAMBLE_SIZE];
labcomm_packet_t labcomm_packet     = { .version        = VERSION,
                                        .destination_id = 0,
                                        .source_id      = 0,
                                        .payload_size   = 0,
                                        .available      = false};
uint8_t the_byte;
Mailbox<0, MAX_BUFFER_SIZE_MESSAGE_SIZE> labcomm_mailbox = {.my_id              = 0,
                                                            .to_id              = 0,
                                                            .from_id            = 0,
                                                            .inbox_msg_size     = 0,
                                                            .outbox_msg_size    = 0,
                                                            .inbox_status       = PACKET_ABSENT,
                                                            .outbox_status      = PACKET_ABSENT,
                                                            .inbox_overflow     = false
                                                            };
/****************************************************************************
 * Main Parser State Machine.                                               *
 * Data will be distributed as complete packets come in.                    *
 * *** WARNING *** DO NOT USE DATA UNLESS .packet_status = PACKET_PRESENT   *
 ****************************************************************************/
void labcomm_parse(uint8_t incoming_byte)
{
    the_byte = incoming_byte;
    switch(parser_state)
    {
        case IDLE:                                          break;
        case GET_VERSION:           get_version();          break;
        case GET_DESTINATION_ID:    get_destination_id();   break;
        case GET_SOURCE_ID:         get_source_id();        break;
        case GET_PAYLOAD_SIZE:      get_payload_size();     break;
        case GET_PAYLOAD:           get_payload();          break;
        case GET_ENDAMBLE:          get_endamble();         break;
    }
    if (parser_state != GET_PAYLOAD)    // Ignore the preamble within the payload
        detect_preamble();              // Detect asynchronous start of packet detection
}
/****************************************************************************
 * Clear Buffer Counters. These all belong to the parser state machine      *
 ****************************************************************************/
void reset_state_machine()
{
    endamble_byte_count         = 0;
    source_id_byte_count        = 0;
    receiver_id_byte_count      = 0;
    payload_size_byte_count     = 0;
    payload_index               = 0;
    labcomm_packet.payload_size = 0;
}
/****************************************************************************
 * Detect Preamble                                                          *
 ****************************************************************************/
void detect_preamble()
{
    static uint8_t preamble_index = 0;      // Only needed to populate the buffer until full
    if (preamble_index < PREAMBLE_SIZE)     // Should stop at sizeof(PREAMBLE) (7)
    {
        preamble_buffer[preamble_index] = the_byte;         // Populate the buffer
        preamble_index++;                                   // until full
    }
    else
    {
        for (uint8_t index=0; index<=PREAMBLE_SIZE-1; index++)
            preamble_buffer[index] = preamble_buffer[index+1];
        preamble_buffer[PREAMBLE_SIZE-1] = the_byte;
    }
    if (!memcmp(preamble_buffer, PREAMBLE, PREAMBLE_SIZE))
    {
        parser_state = GET_VERSION;
        reset_state_machine();
    }
}
/****************************************************************************
 * Get Parser Version                                                       *
 ****************************************************************************/
void get_version()
{
    labcomm_packet.version = the_byte;
    parser_state = GET_DESTINATION_ID;
}
/****************************************************************************
 * Get Destination Module ID                                                *
 ****************************************************************************/
void get_destination_id()
{
    if (receiver_id_byte_count == 0)
    {
        labcomm_packet.destination_id = the_byte * 256;
        receiver_id_byte_count++;
    }
    else
    {
        labcomm_packet.destination_id += the_byte;
        parser_state = GET_SOURCE_ID;
    }
}
/****************************************************************************
 * Get Source Module ID                                                     *
 ****************************************************************************/
void get_source_id()
{
    if (source_id_byte_count == 0)
    {
        labcomm_packet.source_id = the_byte * 256;
        source_id_byte_count++;
    }
    else
    {
        labcomm_packet.source_id += the_byte;
        parser_state = GET_PAYLOAD_SIZE;
    }
}
/****************************************************************************
 * Get Payload Size                                                         *
 ****************************************************************************/
void get_payload_size()
{
    switch(payload_size_byte_count)
    {
        case 0: labcomm_packet.payload_size += the_byte * 256 * 256 * 256;  break;
        case 1: labcomm_packet.payload_size += the_byte * 256 * 256;        break;
        case 2: labcomm_packet.payload_size += the_byte * 256;              break;
        case 3: labcomm_packet.payload_size += the_byte;                    break;
    }
    payload_size_byte_count++;
    if (payload_size_byte_count == PAYLOAD_SIZE_LENGTH)
    {
        if (labcomm_packet.payload_size > LABCOMM_BUFFER_SIZE)
        {
            memcpy(labcomm_mailbox.outbox, MAX_BUFFER_SIZE_MESSAGE, MAX_BUFFER_SIZE_MESSAGE_SIZE);
            labcomm_mailbox.outbox_msg_size = MAX_BUFFER_SIZE_MESSAGE_SIZE;
            labcomm_mailbox.outbox_status = PACKET_PRESENT;
            parser_state = IDLE; // Abort reading payload and endamble
        }
        else parser_state = GET_PAYLOAD;
    }
}
/****************************************************************************
 * Get Payload                                                              *
 ****************************************************************************/
void get_payload()
{
    if (labcomm_packet.payload_size == 0)  // Supports payload length of 0
    {
        parser_state = GET_ENDAMBLE;        // Hacky state jump needed
        get_endamble();                     // Byte was already consumed
    }
    else 
    {
        if (payload_index < labcomm_packet.payload_size)
            labcomm_packet.payload[payload_index++] = the_byte;
        if (payload_index == labcomm_packet.payload_size)
            parser_state = GET_ENDAMBLE;
    }
}
/****************************************************************************
 * Get Enadamble                                                            *
 ****************************************************************************/
void get_endamble()
{
    endamble_buffer[endamble_byte_count] = the_byte;
    endamble_byte_count++;
    if (endamble_byte_count == ENDAMBLE_SIZE)
    {
        parser_state = IDLE;
        if (!memcmp(endamble_buffer, ENDAMBLE, ENDAMBLE_SIZE))
            labcomm_packet.available = true;
    }
}
/****************************************************************************
 * Process Serial                                                           *
 ****************************************************************************/
void process_serial()
{
    if (SerialUSB)
        while (SerialUSB.available() && !labcomm_packet.available)  // Leave to consume old packet
            labcomm_parse(SerialUSB.read());                        // before retreiving new one.
}