/****************************************************************************
 * DC2038A SMBus Services                                                   *
 * Steve Martin                                                             *
 * June 5, 2024                                                             *
 ****************************************************************************/
#ifndef DC2038A_SMBUS_SERVICES_H
#define DC2038A_SMBUS_SERVICES_H

#include "DC2038A_smbus.h"
#include "DC2038A_postoffice.h"          // Gets Mailbox
#include "DC2038A_smbus_comnd_struct.h"  // Defines SMBus payload menu structure

void set_register_list();
void read_register_list();
void enable_stream_mode(); 
void disable_stream_mode();
void write_register_list();
void smbus_read_register();
void smbus_write_register();
void DC2038A_service_smbus();
void set_reg_list_and_stream();
void set_reg_list_and_read_list();

#endif