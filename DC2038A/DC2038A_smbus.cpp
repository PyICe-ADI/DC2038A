/****************************************************************************
 * DC2038A SMBus Phy                                                        *
 * Steve Martin (From D. Simmons)                                           *
 * June 5, 2024                                                             *
 ****************************************************************************/
 #include "DC2038A_smbus.h"
 // Uses the Wire library from Arduino
 // https://www.arduino.cc/reference/en/language/functions/communication/wire/
/****************************************************************************
 * SMBus Read Byte/Word                                                     *
 ****************************************************************************/
SMBUS_reply read_register(uint8_t address, uint8_t command_code, uint8_t use_pec, uint8_t data_size)
{
    uint8_t wire_end_error, pec=0;
    SMBUS_reply reply={.status=0, .lo_byte=0, .hi_byte=0};

    Wire.beginTransmission(address);                                                                        // START followed by chip ADDRESS.
    Wire.write(command_code);                                                                               // Clock in the command code.
    wire_end_error = Wire.endTransmission(false);                                                           // "false" argument causes a RESTART. Keeps bus active for restart to avoid clearing DUT PEC buffer.
    Wire.requestFrom(address, use_pec ? data_size==WORD_SIZE ? 3 : 2 : data_size==WORD_SIZE ? 2 : 1, true); // Clocks 2 bytes/words worth of data plus optional PEC byte from the target device. "true" generates a STOP.
    delayMicroseconds(50);                                                                                  // Put a modicum of air between repeated transactions for scope debug.
    reply.lo_byte = Wire.read();                                                                            // Pulls a byte out of the buffer.
    if (data_size==WORD_SIZE)
        reply.hi_byte = Wire.read();                                                                        // Pulls another byte out of the buffer.
    if (use_pec)
    {
        pec = Wire.read();
        if (data_size==BYTE_SIZE)
            reply.status = pec_read_byte_test(address, command_code, reply.lo_byte, pec)==0 ? 0 : SMBUS_PEC_VALUE_ERROR; // Pulls another byte out of the buffer.
        else
            reply.status = pec_read_word_test(address, command_code, reply.hi_byte<<8 | reply.lo_byte, pec)==0 ? 0 : SMBUS_PEC_VALUE_ERROR; // Pulls another byte out of the buffer.
    }
    switch(wire_end_error)
    {
        case WIRE_END_SUCCESS           :reply.status |= SMBUS_SUCCESS;         break;
        case WIRE_END_BUFFER_OVERFLOW   :reply.status |= SMBUS_BUFFER_OVERFLOW; break;
        case WIRE_END_NACK_ON_ADDRESS   :reply.status |= SMBUS_NACK_ON_ADDRESS; break;
        case WIRE_END_NACK_ON_DATA      :reply.status |= SMBUS_NACK_ON_DATA;    break;
        case WIRE_END_OTHER_ERROR       :reply.status |= SMBUS_UNKNOWN_ERROR;   break;
        case WIRE_END_TIMEOUT           :reply.status |= SMBUS_SMBUS_TIMEOUT;   break;
        default                         :reply.status |= SMBUS_UNKNOWN_ERROR;   break;
    }
    return reply;
}
/****************************************************************************
 * SMBus Write Byte/Word                                                    *
 ****************************************************************************/
SMBUS_reply write_register(uint8_t address, uint8_t command_code, uint8_t use_pec, uint8_t data_size, uint8_t lo_byte, uint8_t hi_byte)
{
    uint8_t wire_end_error;
    SMBUS_reply reply={.status=0, .lo_byte=0, .hi_byte=0};

    Wire.beginTransmission(address);
    Wire.write(command_code);
    Wire.write(lo_byte);
    if (data_size==WORD_SIZE) Wire.write(hi_byte);
    if (use_pec)
    {
        if (data_size==BYTE_SIZE)
            Wire.write(pec_write_byte(address, command_code, lo_byte));
        else
            Wire.write(pec_write_word(address, command_code, hi_byte<<8 | lo_byte));
    }
    wire_end_error = Wire.endTransmission(true);
    switch(wire_end_error)
    {
        case WIRE_END_SUCCESS           :reply.status |= SMBUS_SUCCESS;         break;
        case WIRE_END_BUFFER_OVERFLOW   :reply.status |= SMBUS_BUFFER_OVERFLOW; break;
        case WIRE_END_NACK_ON_ADDRESS   :reply.status |= SMBUS_NACK_ON_ADDRESS; break;
        case WIRE_END_NACK_ON_DATA      :reply.status |= SMBUS_NACK_ON_DATA;    break;
        case WIRE_END_OTHER_ERROR       :reply.status |= SMBUS_UNKNOWN_ERROR;   break;
        case WIRE_END_TIMEOUT           :reply.status |= SMBUS_SMBUS_TIMEOUT;   break;
        default                         :reply.status |= SMBUS_UNKNOWN_ERROR;   break;
    }
    return reply;
}