/****************************************************************************
 * DC2038A Identify Onseself                                                *
 * Steve Martin                                                             *
 * June 5, 2024                                                             *
 ****************************************************************************/
#ifndef DC2038A_IDENTIFY_B
#define DC2038A_IDENTIFY_B

#include "DC2038A_postoffice.h"  // Gets Mailbox
#include "DC2038A_idstring.h"    // Gets Identify message and sizes

void DC2038A_identify();
void send_identity();
void write_scratchpad();
void read_scratchpad();
void get_serialnum();

#endif