/****************************************************************************
 * Labcomm                                                                  *
 * Steve Martin                                                             *
 * June 1, 2023                                                             *
 ****************************************************************************/
#ifndef LABCOMM_H
#define LABCOMM_H

#include <avr/pgmspace.h>
#include <Arduino.h>

const PROGMEM uint8_t PREAMBLE[] = {'L','@','B','C','0','M','m','A','D','I'};
const PROGMEM uint8_t ENDAMBLE[] = {'T','e','o','M'};
const PROGMEM char MAX_BUFFER_SIZE_MESSAGE[] = "*** Labcomm message rejected, exceeds MAX_INCOMING_PAYLOAD_SIZE ***";

const PROGMEM uint8_t VERSION =         1;
#define LABCOMM_BUFFER_SIZE             4096 // Max supported incoming payload size.
#define VERSION_LENGTH                  1
#define SOURCE_ID_LENGTH                2
#define RECEIVER_ID_LENGTH              2
#define PAYLOAD_SIZE_LENGTH             4
#define PREAMBLE_SIZE                   sizeof(PREAMBLE)
#define ENDAMBLE_SIZE                   sizeof(ENDAMBLE)
#define MAX_BUFFER_SIZE_MESSAGE_SIZE    sizeof(MAX_BUFFER_SIZE_MESSAGE)

typedef enum {
        IDLE,               // Waiting for a preamble.
        GET_VERSION,        // Get the version of this protocol.
        GET_DESTINATION_ID, // Get the ID of the target client.
        GET_SOURCE_ID,      // Get the ID of the sender.
        GET_PAYLOAD_SIZE,   // Get the payload size.
        GET_PAYLOAD,        // Get the payload.
        GET_ENDAMBLE        // Get the Endamble.
        } PARSER_STATES;

typedef enum {
        PACKET_ABSENT,
        PACKET_PRESENT,
        } packet_states;

typedef struct {
        uint8_t	        version;
        uint16_t        destination_id;
        uint16_t        source_id;
        uint32_t        payload_size;
        bool            available;
        uint8_t         payload[LABCOMM_BUFFER_SIZE];
        } labcomm_packet_t;

/****************************************************************************
 * Master Mailbox Class                                                     *
 ****************************************************************************/
template <uint16_t inbox_size, uint16_t outbox_size> // Max mailbox size 65,535 Bytes
class Mailbox{
    public:
        uint16_t        my_id;
        uint16_t        to_id;
        uint16_t        from_id;
        uint32_t        inbox_msg_size;
        uint32_t        outbox_msg_size;
        uint8_t         inbox_status;
        uint8_t         outbox_status;
        bool            inbox_overflow;
        uint8_t         inbox[inbox_size];
        uint8_t         outbox[outbox_size];
        /*************************
         * Retrieve Data         *
         *************************/
        void get_packet(labcomm_packet_t* incoming_packet)
        {
            if (inbox_status == PACKET_PRESENT) {inbox_overflow = true; return;}        // No soup for you!!! Have the client clear inbox_overflow!!!
            if (incoming_packet->payload_size <= sizeof(inbox))                         // Disallow RAM nuking by adjacent memory overwrite of penpal
            {                                                                           // Otherwise ignore
                memcpy(inbox, incoming_packet->payload, incoming_packet->payload_size); // Grab the payload
                from_id         = incoming_packet->source_id;                           // Grab the sender ID
                inbox_msg_size  = incoming_packet->payload_size;                        // Grab the size
                inbox_status    = PACKET_PRESENT;                                       // Declare the packet present
            }                                                                           // Always flag as message read
            incoming_packet->available = false;                                         // Packet consumed. Clear the packet available flag
        }
        /*************************
         * Send Data             *
         *************************/
        void send_packet()
        {
            SerialUSB.write(PREAMBLE, PREAMBLE_SIZE);
            SerialUSB.write(VERSION);
            SerialUSB.write(highByte(to_id)); // Destination ID
            SerialUSB.write( lowByte(to_id));
            SerialUSB.write(highByte(my_id)); // Source ID
            SerialUSB.write( lowByte(my_id));
            SerialUSB.write((outbox_msg_size >> 24) & 0xff);
            SerialUSB.write((outbox_msg_size >> 16) & 0xff);
            SerialUSB.write((outbox_msg_size >> 8)  & 0xff);
            SerialUSB.write((outbox_msg_size >> 0)  & 0xff);
            for (unsigned long index=0; index < outbox_msg_size; index++) SerialUSB.write(outbox[index]);
            SerialUSB.write(ENDAMBLE, ENDAMBLE_SIZE);
            outbox_status = PACKET_ABSENT;
        }
};
/****************************************************************************
 *                                                                          *
 ****************************************************************************/

void labcomm_parse(uint8_t);
void reset_state_machine();
void get_version();
void get_source_id();
void get_destination_id();
void get_payload_size();
void get_payload();
void get_endamble();
void detect_preamble();
void process_serial();

extern Mailbox<0, MAX_BUFFER_SIZE_MESSAGE_SIZE> labcomm_mailbox;
extern labcomm_packet_t labcomm_packet;

#endif