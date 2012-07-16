#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include "contiki.h"

//! Enable the bluetooth device
//! Any other devices on the same bus will be disabled
//! @param connect_handler  This function will be called when the connect
//!                         state changes.
//! @param data_handler     This function will be called whenever a byte
//!                         has been received
void bluetooth_enable(void (*connect_handler)(unsigned char connected), int (*data_handler)(unsigned char c));

//! Disable the bluetooth device
void bluetooth_disable(void);

//! Calls configured connect_handler
//! This function should be called from ISR
//! @param connected        Connection state of the device
void bluetooth_set_connected(int connected);

#endif // BLUETOOTH_H
