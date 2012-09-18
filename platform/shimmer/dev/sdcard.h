#ifndef SDCARD_H
#define SDCARD_H

#include "contiki.h"

enum SDCardStatus
{
  SDCARD_SUCCESS,
  SDCARD_ERROR
};

void sdcard_start(void);
void sdcard_stop(void);
void sdcard_restart(void);
void
sdcard_send_command(const uint8_t cmd, uint32_t data, const uint8_t crc);

enum SDCardStatus sdcard_initialize(void);

#endif // SDCARD_H
