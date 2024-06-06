/****************************************************************************
 * Demo Board for the LTC4162 Family of Products                            *
 * Steve Martin                                                             *
 * June 5, 2024                                                            	*
 ****************************************************************************/
#include "DC2038A.h"

/****************************************************************************
 * Setup                                                                    *
 ****************************************************************************/
void setup()
{
    Wire.begin();
    SerialUSB.begin(115200);
    pinMode(HEARTBEAT_LED, OUTPUT);
    pinMode(SMBALERTPIN, INPUT);
}
/****************************************************************************
 * Loop                                                                     *
 ****************************************************************************/
void loop() // Simple and fast round robin operating system
{
    process_serial();
    DC2038A_identify();
    DC2038A_heartbeat();
    DC2038A_process_mail();
    DC2038A_service_smbus();
    DC2038A_get_smbalert_pin();
}