#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include "contiki.h"

//! Enable the bluetooth device
//!
//! @param connect_handler  This function will be called when the connect
//!                         state changes.
void bluetooth_enable(void (*connect_handler)(unsigned char connected));

//! Disable the bluetooth device
void bluetooth_disable(void);

//! Enables communication to the bluetooth device
//!
//! @note You should only call this function after all other devices on UART1
//!       have been disabled!
//! @param data_handler This function will be called whenever a byte
//!                     has been received.
void bluetooth_enable_communication(int (*data_handler)(unsigned char c));

//! Disables communication to the bluetooth device
//!
//! After calling this function it is safe to use UART1 for SPI communication.
//! The bluetooth module stays enabled and buffers incoming data.
void bluetooth_disable_communication();

//! Send a single byte over bluetooth
//!
//! @param b A byte to send over bluetooth
void bluetooth_writeb(unsigned char b);

uint8_t bluetooth_active();
void bluetooth_isr();
void bluetooth_rts_isr();

#endif // BLUETOOTH_H
